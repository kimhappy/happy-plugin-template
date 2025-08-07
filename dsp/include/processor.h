#pragma once

#include <shared_mutex>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <util.hpp>

namespace happy {
  class Processor:
    public juce::AudioProcessor {
  public:
    Processor() noexcept;

    // Information
    auto getName() const noexcept -> juce::String const override;
    auto acceptsMidi() const noexcept -> bool override;
    auto producesMidi() const noexcept -> bool override;
    auto isMidiEffect() const noexcept -> bool override;
    auto isBusesLayoutSupported(BusesLayout const& layouts) const noexcept -> bool override;
    auto getTailLengthSeconds() const noexcept -> double override;

    // Editor
    auto hasEditor() const noexcept -> bool override;
    auto createEditor() noexcept -> juce::AudioProcessorEditor* override;

    // Program
    auto getNumPrograms() noexcept -> int32_t override;
    auto getCurrentProgram() noexcept -> int32_t override;
    auto setCurrentProgram(int32_t index) noexcept -> void override;
    auto getProgramName(int32_t index) noexcept -> juce::String const override;
    auto changeProgramName(
      int32_t             index,
      juce::String const& newName
    ) noexcept -> void override;

    // State
    auto getStateInformation(juce::MemoryBlock& destData) noexcept -> void override;
    auto setStateInformation(
      void    const* data,
      int32_t        sizeInBytes
    ) noexcept -> void override;

    template< typename AP >
    auto getState(util::Parameter< AP > const& id) const noexcept -> AP const& {
      return static_cast< AP const& >(*_state.getParameter(id));
    }

    template< typename AP >
    auto getState(util::Parameter< AP > const& id) noexcept -> AP& {
      return static_cast< AP& >(*_state.getParameter(id));
    }

    // Process
    auto prepareToPlay(
      double  sampleRate,
      int32_t samplesPerBlock
    ) noexcept -> void override;
    auto releaseResources() noexcept -> void override;
    auto processBlock(
      juce::AudioBuffer< float >&,
      juce::MidiBuffer          &
    ) noexcept -> void override;

    // Output
    mutable std::shared_mutex levelsMutex;
    std::vector< float > levels;

  private:
    juce::AudioProcessorValueTreeState          _state;
    juce::dsp::BallisticsFilter       < float > _envelopeFollower;
    juce::AudioBuffer                 < float > _envelopeFollowerOutputBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)
  };
} // namespace happy
