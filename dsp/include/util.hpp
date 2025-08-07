#pragma once

#include <ranges>
#include <juce_audio_processors/juce_audio_processors.h>
#include <type_traits>

namespace happy::util {
  template< typename AP >
  struct Parameter {
  private:
    juce::ParameterID _id;

  public:
    constexpr Parameter(auto&& id) noexcept:
      _id(std::forward< decltype(id) >(id)) {}

    constexpr operator juce::ParameterID() const noexcept {
      return _id;
    }

    constexpr operator juce::StringRef() const noexcept {
      return _id.getParamID();
    }
  };

  constexpr auto stringToVector(juce::String const& string) noexcept -> std::vector< std::byte > {
    auto const ptr  = (std::byte*)string.toRawUTF8();
    auto const size = string.getNumBytesAsUTF8();
    return { ptr, ptr + size };
  }

  constexpr auto extensionToMime(juce::String const& extension) noexcept -> juce::String {
    static auto const MIME_MAP = std::unordered_map< juce::String, juce::String > {
      { "htm"  , "text/html"                },
      { "html" , "text/html"                },
      { "txt"  , "text/plain"               },
      { "jpg"  , "image/jpeg"               },
      { "jpeg" , "image/jpeg"               },
      { "svg"  , "image/svg+xml"            },
      { "ico"  , "image/vnd.microsoft.icon" },
      { "json" , "application/json"         },
      { "png"  , "image/png"                },
      { "css"  , "text/css"                 },
      { "map"  , "application/json"         },
      { "js"   , "text/javascript"          },
      { "woff2", "font/woff2"               } };

    if (auto const it = MIME_MAP.find(extension.toLowerCase()); it != MIME_MAP.end())
      return it->second;

    return {};
  }

  constexpr auto resourceProviderRoot() noexcept -> juce::String {
#ifdef SERVER_BUNDLE
    return juce::WebBrowserComponent::getResourceProviderRoot();
#else
    auto const port = juce::SystemStats::getEnvironmentVariable("HAPPY_PORT", "3000");
    return "http://localhost:" + port;
#endif // SERVER_BUNDLE
  }

  template< typename Data >
  constexpr auto bundledResourceProvider(Data const& data) noexcept {
    return [data](juce::String const& fileName) noexcept -> std::optional< std::vector< std::byte > > {
      auto const fileNameView = std::u8string_view(reinterpret_cast< char8_t const* >(fileName.toRawUTF8()));
      auto const fileSpan     = data(fileNameView);

      if (fileSpan)
        return fileSpan.value() | std::ranges::to< std::vector< std::byte > >();

      return std::nullopt;
    };
  }

  template< typename... Providers >
  class ResourceProvider {
  private:
    std::tuple< Providers... > _providers;

  public:
    template< typename... Args >
    ResourceProvider(Args&&... args) noexcept:
      _providers(std::forward< Args >(args)...) {}

    constexpr auto operator()(juce::String const& url) noexcept -> std::optional< juce::WebBrowserComponent::Resource > {
      auto const fileName  = url == "/" ? "index.html" : url.fromFirstOccurrenceOf("/", false, false);
      auto const extension = fileName.fromLastOccurrenceOf(".", false, false);
      auto const mime      = extensionToMime(extension);

      if (mime.isEmpty())
        return std::nullopt;

      auto const resource = [&]< auto... Is >(std::index_sequence< Is... >) noexcept -> std::optional< std::vector< std::byte > > {
        auto result = std::optional< std::vector< std::byte > >();
        ((result = std::get< Is >(_providers)(fileName)) || ...);
        return result;
      } (std::index_sequence_for< Providers... >());

      if (!resource)
        return std::nullopt;

      return juce::WebBrowserComponent::Resource {
        std::move(*resource),
        std::move(mime)
      };

      return std::nullopt;
    }
  };

  template< typename... Args >
  ResourceProvider(Args&&... args) noexcept -> ResourceProvider< std::decay_t< Args >... >;
} // namespace happy::util
