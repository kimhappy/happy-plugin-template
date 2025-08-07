#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <processor.h>

namespace happy {
  class Editor:
    public  juce::AudioProcessorEditor,
    private juce::Timer {
  public:
    Editor(Processor&) noexcept;

    auto resized() noexcept -> void override;
    auto timerCallback() noexcept -> void override;

  private:
    auto _customResourceProvider(juce::String const& fileName) noexcept -> std::optional< std::vector< std::byte > >;
    auto _handleJsEvent(juce::var const& objectFromFrontend) noexcept -> void;
    auto _nativeFunction(
      juce::Array< juce::var >                            const& args,
      juce::WebBrowserComponent::NativeFunctionCompletion const& completion
    ) noexcept -> void;
    auto _runJs(juce::String const& jsCode) noexcept -> void;
    auto _emitJsEvent(
      juce::Identifier const& eventId,
      juce::var        const& eventData
    ) noexcept -> void;

    Processor& _processorRef;

    juce::TextButton _runJavaScriptButton;
    juce::TextButton _emitJavaScriptEventButton;
    juce::Label      _labelUpdatedFromJavaScript;

    juce::Slider                       _gainSlider;
    juce::SliderParameterAttachment    _gainSliderAttachment;
    juce::WebSliderRelay               _webGainRelay;
    juce::WebSliderParameterAttachment _webGainSliderAttachment;

    juce::ToggleButton                       _bypassButton;
    juce::ButtonParameterAttachment          _bypassButtonAttachment;
    juce::WebToggleButtonRelay               _webBypassRelay;
    juce::WebToggleButtonParameterAttachment _webBypassToggleAttachment;

    juce::Label                          _distortionTypeLabel;
    juce::ComboBox                       _distortionTypeComboBox;
    juce::ComboBoxParameterAttachment    _distortionTypeComboBoxAttachment;
    juce::WebComboBoxRelay               _webDistortionTypeRelay;
    juce::WebComboBoxParameterAttachment _webDistortionTypeComboBoxAttachment;

    juce::WebBrowserComponent _webView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
  };
}  // namespace happy
