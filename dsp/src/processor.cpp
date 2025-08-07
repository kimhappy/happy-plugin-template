#include <processor.h>
#include <editor.h>
#include <parameter.hpp>

namespace happy {
  Processor::Processor() noexcept:
    AudioProcessor(
      BusesProperties()
#if !JucePlugin_IsMidiEffect
#  if !JucePlugin_IsSynth
        .withInput("Input"  , juce::AudioChannelSet::stereo(), true)
#  endif // !JucePlugin_IsSynth
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif // !JucePlugin_IsMidiEffect
    ),
    _state(*this, nullptr, "PARAMETERS", {
      std::make_unique< juce::AudioParameterFloat >(
        parameter::GAIN, "gain",
        juce::NormalisableRange< float >(0.f, 1.f, 0.01f, 0.9f), 1.f),
      std::make_unique< juce::AudioParameterBool >(
        parameter::BYPASS, "bypass", false,
        juce::AudioParameterBoolAttributes().withLabel("Bypass")),
      std::make_unique< juce::AudioParameterChoice >(
        parameter::DISTORTION_TYPE, "distortion type",
        juce::StringArray { "none", "tanh(kx)/tanh(k)", "sigmoid" }, 0)
    }) {}

  auto Processor::getName() const noexcept -> juce::String const {
    return JucePlugin_Name;
  }

  auto Processor::acceptsMidi() const noexcept -> bool {
    return JucePlugin_WantsMidiInput;
  }

  auto Processor::producesMidi() const noexcept -> bool {
    return JucePlugin_ProducesMidiOutput;
  }

  auto Processor::isMidiEffect() const noexcept -> bool {
    return JucePlugin_IsMidiEffect;
  }

  auto Processor::isBusesLayoutSupported(BusesLayout const& layouts) const noexcept -> bool {
    auto const isSurround =
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono  () &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo();
    auto const isEndomorphism =
      layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();

    return JucePlugin_IsMidiEffect || (!isSurround && (JucePlugin_IsSynth || isEndomorphism));
  }

  auto Processor::getTailLengthSeconds() const noexcept -> double {
    return 0.0;
  }

  auto Processor::hasEditor() const noexcept -> bool {
    return true;
  }

  auto Processor::createEditor() noexcept -> juce::AudioProcessorEditor* {
    return new Editor(*this);
  }

  auto Processor::getNumPrograms() noexcept -> int32_t {
    return 1;
  }

  auto Processor::getCurrentProgram() noexcept -> int32_t {
    return 0;
  }

  auto Processor::setCurrentProgram(int32_t const index) noexcept -> void {
    juce::ignoreUnused(index);
  }

  auto Processor::getProgramName(int32_t const index) noexcept -> juce::String const {
    juce::ignoreUnused(index);
    return {};
  }

  auto Processor::changeProgramName(
    int32_t      const  index,
    juce::String const& newName
  ) noexcept -> void {
    juce::ignoreUnused(index, newName);
  }

  auto Processor::getStateInformation(juce::MemoryBlock& destData) noexcept -> void {
    juce::ignoreUnused(destData);
  }

  auto Processor::setStateInformation(
    void const* const data,
    int32_t     const sizeInBytes
  ) noexcept -> void {
    juce::ignoreUnused(data, sizeInBytes);
  }

  auto Processor::prepareToPlay(
    double  const sampleRate,
    int32_t const samplesPerBlock
  ) noexcept -> void {
    _envelopeFollower.prepare(
      juce::dsp::ProcessSpec {
        .sampleRate       = sampleRate                              ,
        .maximumBlockSize = static_cast< uint32_t >(samplesPerBlock),
        .numChannels      = static_cast< uint32_t >(getTotalNumOutputChannels())
      }
    );
    _envelopeFollower.setAttackTime          (200.f);
    _envelopeFollower.setReleaseTime         (200.f);
    _envelopeFollower.setLevelCalculationType(juce::dsp::BallisticsFilter< float >::LevelCalculationType::peak);

    _envelopeFollowerOutputBuffer.setSize(
      getTotalNumOutputChannels(),
      samplesPerBlock
    );

    auto const lock = std::unique_lock(levelsMutex);
    levels.resize(static_cast< size_t >(getTotalNumOutputChannels()), 0.f);
  }

  auto Processor::releaseResources() noexcept -> void {}

  auto Processor::processBlock(
    juce::AudioBuffer< float >& buffer,
    juce::MidiBuffer          & midiMessages
  ) noexcept -> void {
    juce::ignoreUnused(midiMessages);

    auto const noDenormals       = juce::ScopedNoDenormals  ();
    auto const numInputChannels  = getTotalNumInputChannels ();
    auto const numOutputChannels = getTotalNumOutputChannels();
    auto const numSamples        = buffer.getNumSamples();

    auto const& bypassParameter         = getState(parameter::BYPASS         );
    auto const& distortionTypeParameter = getState(parameter::DISTORTION_TYPE);
    auto const& gainParameter           = getState(parameter::GAIN           );

    for (auto ch = numInputChannels; ch < numOutputChannels; ++ch)
      buffer.clear(ch, 0, numSamples);

    if (bypassParameter.get() || numSamples == 0)
      return;

    auto const block = juce::dsp::AudioBlock< float >(buffer);

    if (distortionTypeParameter.getIndex() == 1) {
      juce::dsp::AudioBlock< float >::process(block, block, [](float const sample) noexcept {
        static constexpr auto const SATURATION    = 5.f;
        static           auto const NORMALIZATION = std::tanh(SATURATION);
        return std::tanh(SATURATION * sample) / NORMALIZATION;
      });
    }
    else if (distortionTypeParameter.getIndex() == 2) {
      juce::dsp::AudioBlock< float >::process(block, block, [](float const sample) noexcept {
        static constexpr auto const SATURATION = 5.f;
        return 2.f / (1.f + std::exp(-SATURATION * sample)) - 1.f;
      });
    }

    buffer.applyGain(gainParameter.get());

    auto const inBlock =
      juce::dsp::AudioBlock< float >(buffer).getSubsetChannelBlock(
        0u, static_cast< size_t >(numOutputChannels));
    auto outBlock =
      juce::dsp::AudioBlock< float >(_envelopeFollowerOutputBuffer).getSubBlock(
        0u, static_cast< size_t >(numSamples));
    _envelopeFollower.process(
      juce::dsp::ProcessContextNonReplacing< float >(inBlock, outBlock));

    auto const lock = std::unique_lock(levelsMutex);

    for (auto ch = 0; ch < numOutputChannels; ++ch)
      levels[ static_cast< size_t >(ch) ] =
        juce::Decibels::gainToDecibels(
          outBlock.getSample(ch, static_cast< int32_t >(outBlock.getNumSamples()) - 1));
  }
} // namespace happy

auto JUCE_CALLTYPE createPluginFilter() -> juce::AudioProcessor* {
  return new happy::Processor();
}
