cmake_minimum_required(VERSION 3.24.1)

set(PROJECT_NAME "SpectrumAnalyzer")

project(${PROJECT_NAME} VERSION 1)

set(FORMATS VST3 Standalone)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

add_subdirectory(External/JUCE)

juce_add_plugin("${PROJECT_NAME}"
    COMPANY_NAME "Vain Audio"
    COMPANY_WEBSITE "www.vainaudio.com"
    BUNDLE_ID com.VainAudio.VainAudio
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Vain
    PLUGIN_CODE VaSA
    FORMATS "${FORMATS}"
    PRODUCT_NAME "Vain Audio Spectrum Analyzer")
	
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

if(MSVC)
  target_compile_options("${PROJECT_NAME}" PUBLIC /W4 /WX)
else()
  target_compile_options("${PROJECT_NAME}" PUBLIC -Wall -Wextra -Wpedantic -g -Werror)
  target_link_options("${PROJECT_NAME}" PUBLIC -g)
endif()

set(SpectrumAnalyzerSourceFiles
		Source/SpectrumAnalyzer/SpectrumAnalyzer.cpp
		Source/SpectrumAnalyzer/SpectrumAnalyzerComponent.cpp
		Source/SpectrumAnalyzer/AudioBufferFifo.cpp
		Source/SpectrumAnalyzer/SpectrumAnalyzerCurve.cpp
		Source/SpectrumAnalyzer/SpectrumAnalyzerBuffer.cpp
		Source/SpectrumAnalyzer/SpectrumAnalyzerAverager.cpp
		Source/SpectrumAnalyzer/SpectrumAnalyzerBinSmoother.cpp
)

set(SourceFiles
		Source/PluginEditor.cpp
		Source/PluginProcessor.cpp

		"${SpectrumAnalyzerSourceFiles}"
	)

target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

target_include_directories("${PROJECT_NAME}" PUBLIC Source)

target_compile_definitions("${PROJECT_NAME}"
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries("${PROJECT_NAME}"
	PUBLIC
    juce::juce_audio_utils
    juce::juce_audio_processors
	juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

# ------------------------ tests ------------------------

enable_testing()

add_subdirectory(External/googletest)

set_target_properties(gmock PROPERTIES FOLDER External)
set_target_properties(gmock_main PROPERTIES FOLDER External)
set_target_properties(gtest PROPERTIES FOLDER External)
set_target_properties(gtest_main PROPERTIES FOLDER External)
set_target_properties(${PROJECT_NAME}_rc_lib PROPERTIES FOLDER External)

set(TestSourceFiles
	Tests/Tests.cpp
)

add_executable(${PROJECT_NAME}_Tests ${TestSourceFiles})
target_compile_features(${PROJECT_NAME}_Tests PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME}_Tests
    PRIVATE
	${PROJECT_NAME}
    gtest)

target_include_directories(${PROJECT_NAME}_Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_compile_definitions(${PROJECT_NAME}_Tests INTERFACE $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_COMPILE_DEFINITIONS>)
target_include_directories(${PROJECT_NAME}_Tests INTERFACE $<TARGET_PROPERTY:${PROJECT_NAME},INCLUDE_DIRECTORIES>)

set_target_properties(${PROJECT_NAME}_Tests PROPERTIES FOLDER "Targets")
