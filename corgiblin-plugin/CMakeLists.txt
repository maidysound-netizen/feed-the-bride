cmake_minimum_required(VERSION 3.22)
project(CorgiblinRecorder VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")

include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 9.0.2
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

set(CORGIBLIN_FORMATS VST3 Standalone)
if(APPLE)
  list(APPEND CORGIBLIN_FORMATS AU)
endif()

juce_add_plugin(CorgiblinRecorder
  COMPANY_NAME "Takako World"
  BUNDLE_ID "com.takakoworld.corgiblinrecorder"
  PLUGIN_MANUFACTURER_CODE Taka
  PLUGIN_CODE CgRb
  FORMATS ${CORGIBLIN_FORMATS}
  PRODUCT_NAME "CORGIBLIN RECORDER"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(CorgiblinRecorder PRIVATE
  src/PluginProcessor.cpp
  src/PluginProcessor.h
  src/PluginEditor.cpp
  src/PluginEditor.h
  src/CorgiblinVoice.cpp
  src/CorgiblinVoice.h
  src/CorgiblinSound.h
  src/HeckLayer.h
  src/ParameterIDs.h
)

target_compile_definitions(CorgiblinRecorder PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
)

target_link_libraries(CorgiblinRecorder PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  PUBLIC
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)
