#pragma once

#include <util.hpp>

namespace happy::parameter {
  inline auto const GAIN            = util::Parameter< juce::AudioParameterFloat  > { "GAIN"            };
  inline auto const BYPASS          = util::Parameter< juce::AudioParameterBool   > { "BYPASS"          };
  inline auto const DISTORTION_TYPE = util::Parameter< juce::AudioParameterChoice > { "DISTORTION_TYPE" };
} // namespace happy::parameter
