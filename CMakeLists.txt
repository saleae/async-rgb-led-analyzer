cmake_minimum_required (VERSION 3.11)
project(async_rgb_led_analyzer)

add_definitions( -DLOGIC2 )

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum supported MacOS version" FORCE)

# enable generation of compile_commands.json, helpful for IDEs to locate include files.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES 
src/AsyncRgbLedAnalyzer.cpp
src/AsyncRgbLedAnalyzer.h
src/AsyncRgbLedAnalyzerResults.cpp
src/AsyncRgbLedAnalyzerResults.h
src/AsyncRgbLedAnalyzerSettings.cpp
src/AsyncRgbLedAnalyzerSettings.h
src/AsyncRgbLedHelpers.cpp
src/AsyncRgbLedHelpers.h
src/AsyncRgbLedSimulationDataGenerator.cpp
src/AsyncRgbLedSimulationDataGenerator.h
)

add_analyzer_plugin(async_rgb_led_analyzer SOURCES ${SOURCES})
