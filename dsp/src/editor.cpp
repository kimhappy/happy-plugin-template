#include <ranges>
#include <nlohmann/json.hpp>
#include <editor.h>
#include <parameter.hpp>
#include <util.hpp>

#ifdef SERVER_BUNDLE
#  include <server_files.hpp>
#endif // SERVER_BUNDLE

namespace happy {
  Editor::Editor(Processor& p) noexcept:
    juce::AudioProcessorEditor(&p),
    _processorRef(p),

    _runJavaScriptButton        { "Run some JavaScript"                    },
    _emitJavaScriptEventButton  { "Emit JavaScript event"                  },
    _labelUpdatedFromJavaScript { "label", "To be updated from JavaScript" },

    _gainSlider { "gain slider" },
    _gainSliderAttachment {
      _processorRef.getState(parameter::GAIN),
      _gainSlider
    },
    _webGainRelay { parameter::GAIN },
    _webGainSliderAttachment {
      _processorRef.getState(parameter::GAIN),
      _webGainRelay
    },

    _bypassButton { "Bypass" },
    _bypassButtonAttachment {
      _processorRef.getState(parameter::BYPASS),
      _bypassButton
    },
    _webBypassRelay { parameter::BYPASS },
    _webBypassToggleAttachment {
      _processorRef.getState(parameter::BYPASS),
      _webBypassRelay
    },

    _distortionTypeLabel    { "distortion type label", "Distortion" },
    _distortionTypeComboBox { "distortion type combo box" },
    _distortionTypeComboBoxAttachment {
      _processorRef.getState(parameter::DISTORTION_TYPE),
      _distortionTypeComboBox
    },
    _webDistortionTypeRelay { parameter::DISTORTION_TYPE },
    _webDistortionTypeComboBoxAttachment {
      _processorRef.getState(parameter::DISTORTION_TYPE),
      _webDistortionTypeRelay
    },

    _webView {
      juce::WebBrowserComponent::Options {}
        .withBackend(
          juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options(
          juce::WebBrowserComponent::Options::WinWebView2 {}
            .withBackgroundColour(juce::Colours::white)
            .withUserDataFolder(
              juce::File::getSpecialLocation(
                juce::File::SpecialLocationType::tempDirectory)))
        .withNativeIntegrationEnabled()
        .withResourceProvider(
          util::ResourceProvider(
#ifdef SERVER_BUNDLE
            util::bundledResourceProvider(happy::server::data),
#endif // SERVER_BUNDLE
            std::bind_front(&Editor::_customResourceProvider, this)
          ),
          juce::URL(util::resourceProviderRoot()).getOrigin()
        )
        .withInitialisationData("vendor"        , JucePlugin_Manufacturer )
        .withInitialisationData("pluginName"    , JucePlugin_Name         )
        .withInitialisationData("pluginVersion" , JucePlugin_VersionString)
        .withEventListener     ("exampleJsEvent", std::bind_front(&Editor::_handleJsEvent , this))
        .withNativeFunction    ("nativeFunction", std::bind_front(&Editor::_nativeFunction, this))
        .withUserScript        ("console.log('C++ backend here: This is run before any other loading happens');")
        .withOptionsFrom       (_webGainRelay          )
        .withOptionsFrom       (_webBypassRelay        )
        .withOptionsFrom       (_webDistortionTypeRelay)
    } {
    _webView.goToURL(util::resourceProviderRoot());
    addAndMakeVisible(_webView);

    _runJavaScriptButton.onClick = std::bind(&Editor::_runJs, this, "console.log('Hello from C++!');");
    addAndMakeVisible(_runJavaScriptButton);

    _emitJavaScriptEventButton.onClick = std::bind(&Editor::_emitJsEvent, this, "exampleEvent", 42.0);
    addAndMakeVisible(_emitJavaScriptEventButton);

    addAndMakeVisible(_labelUpdatedFromJavaScript);

    _gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    addAndMakeVisible(_gainSlider);

    addAndMakeVisible(_bypassButton);

    addAndMakeVisible(_distortionTypeLabel);

    auto const& distortionTypeParameter = _processorRef.getState< juce::AudioParameterChoice >("DISTORTION_TYPE");
    _distortionTypeComboBox.addItemList(
      distortionTypeParameter.choices, 1);
    _distortionTypeComboBox.setSelectedItemIndex(
      distortionTypeParameter.getIndex(),
      juce::dontSendNotification);
    addAndMakeVisible(_distortionTypeComboBox);

    setResizable(true, true);
    setSize(800, 600);
    startTimer(60);
  }

  auto Editor::resized() noexcept -> void {
    auto bounds = getBounds();
    _webView                   .setBounds(bounds.removeFromRight(bounds.getWidth() / 2));
    _runJavaScriptButton       .setBounds(bounds.removeFromTop(50).reduced( 5));
    _emitJavaScriptEventButton .setBounds(bounds.removeFromTop(50).reduced( 5));
    _labelUpdatedFromJavaScript.setBounds(bounds.removeFromTop(50).reduced( 5));
    _gainSlider                .setBounds(bounds.removeFromTop(50).reduced( 5));
    _bypassButton              .setBounds(bounds.removeFromTop(50).reduced(10));
    _distortionTypeLabel       .setBounds(bounds.removeFromTop(50).reduced( 5));
    _distortionTypeComboBox    .setBounds(bounds.removeFromTop(50).reduced( 5));
  }

  auto Editor::timerCallback() noexcept -> void {
    _webView.emitEventIfBrowserIsVisible("outputLevel", juce::var {});
  }

  auto Editor::_customResourceProvider(juce::String const& fileName) noexcept -> std::optional< std::vector< std::byte > > {
    if (fileName == "outputLevel.json") {
      auto const lock      = std::shared_lock(_processorRef.levelsMutex);
      auto const levelData = nlohmann::json { { "levels", _processorRef.levels } };
      return util::stringToVector(levelData.dump());
    }

    return std::nullopt;
  }

  auto Editor::_handleJsEvent(juce::var const& objectFromFrontend) noexcept -> void {
    auto const cnt = objectFromFrontend.getProperty("emittedCount", 0);

    _labelUpdatedFromJavaScript.setText(
      "example JavaScript event occurred with value " + cnt.toString(),
      juce::dontSendNotification);
  }

  auto Editor::_nativeFunction(
    juce::Array< juce::var >                            const& args,
    juce::WebBrowserComponent::NativeFunctionCompletion const& completion
  ) noexcept -> void {
    auto const concatenatedString = std::ranges::fold_left(
      args | std::views::transform(&juce::var::toString),
      juce::String {}, std::plus {});

    _labelUpdatedFromJavaScript.setText(
      "Native function called with args: " + concatenatedString,
      juce::dontSendNotification);

    completion("nativeFunction callback: All OK!");
  }

  auto Editor::_runJs(juce::String const& jsCode) noexcept -> void {
    _webView.evaluateJavascript(jsCode);
  }

  auto Editor::_emitJsEvent(
    juce::Identifier const& eventId,
    juce::var        const& eventData
  ) noexcept -> void {
    _webView.emitEventIfBrowserIsVisible(eventId, eventData);
  }
} // namespace happy
