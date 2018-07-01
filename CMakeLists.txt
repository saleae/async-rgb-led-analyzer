cmake_minimum_required (VERSION 3.11)
project(async_rgb_led_analyzer)

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

add_library(async_rgb_led_analyzer SHARED ${SOURCES})
target_link_libraries(async_rgb_led_analyzer PRIVATE Saleae::AnalyzerSDK)

if(MSVC)
    install(TARGETS async_rgb_led_analyzer RUNTIME DESTINATION ".")
else()
    install(TARGETS async_rgb_led_analyzer LIBRARY DESTINATION ".")
endif()
