cmake_minimum_required ( VERSION 3.22 FATAL_ERROR )
project ( happy-plugin-template VERSION 0.1.0 )

set ( CMAKE_CXX_STANDARD            23 )
set ( CMAKE_CXX_STANDARD_REQUIRED   ON )
set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )

add_subdirectory ( node_modules/happy-juce/JUCE )
add_subdirectory ( node_modules/nlohmann_json   )
add_subdirectory ( node_modules/happy-bundle    )

if ( WIN32 )
  execute_process (
    COMMAND           pwsh -NoProfile -File script/DownloadWebView2.ps1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE   DOWNLOAD_WEBVIEW2_RESULT
  )

  if ( NOT DOWNLOAD_WEBVIEW2_RESULT EQUAL 0 )
    message ( FATAL_ERROR "Failed to download Microsoft.Web.WebView2 NuGet package. Result: ${DOWNLOAD_WEBVIEW2_RESULT}" )
  endif ()
endif ()

if ( DEFINED VIEW_BUILD_DIR )
  file ( GLOB_RECURSE BUNDLE_FILES CONFIGURE_DEPENDS
    "${VIEW_BUILD_DIR}/*"
  )

  happy_bundle ( server_files
    HEADER_NAME server_files.hpp
    NAMESPACE   happy::server
    FILES       "${BUNDLE_FILES}"
  )

  target_compile_definitions ( server_files
    INTERFACE
      SERVER_BUNDLE
  )

else ()
  add_library ( server_files INTERFACE )
endif ()

juce_add_plugin ( ${PROJECT_NAME}
  COMPANY_NAME                  "kimhappy"
  IS_SYNTH                      FALSE
  NEEDS_MIDI_INPUT              FALSE
  NEEDS_MIDI_OUTPUT             FALSE
  PLUGIN_MANUFACTURER_CODE      "Khpy"
  PLUGIN_CODE                   "Hptp"
  FORMATS                       VST3 Standalone
  PRODUCT_NAME                  "happy-plugin-template"
  NEEDS_WEB_BROWSER             TRUE
  NEEDS_WEBVIEW2                TRUE
  MICROPHONE_PERMISSION_ENABLED TRUE
)

file ( GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "dsp/src/*.cpp"
)

target_sources ( ${PROJECT_NAME}
  PRIVATE
    ${SOURCES}
)

target_include_directories ( ${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/include

  SYSTEM PUBLIC
    ${JUCE_MODULES_DIR}
)

target_link_libraries ( ${PROJECT_NAME}
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    nlohmann_json::nlohmann_json
    server_files

  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

target_compile_definitions ( ${PROJECT_NAME}
  PUBLIC
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
)
