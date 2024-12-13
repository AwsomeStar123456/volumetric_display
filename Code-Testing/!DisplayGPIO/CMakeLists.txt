#Ensure that the cmake and compiler is all correct for the pico SDK
cmake_minimum_required(VERSION 3.13)

#Set the project to C and C++ with the correct standards.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Set this path to match the path that your installed pico SDK is located at.
set(PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk")
#Include the SDK import CMake file, this will include all of the required libraries and files.
include(pico_sdk_import.cmake)

#Create the display project and add all of the required files.
project(display C CXX ASM)

#Initialise the pico SDK
pico_sdk_init()

#Initialize the onboard LED path.
set(ONBOARD_LED_PATH "${CMAKE_SOURCE_DIR}/lib/OnboardLED/ws2812.c")
set(ONBOARD_LED_PIO "${CMAKE_SOURCE_DIR}/lib/OnboardLED/ws2812.pio")

# Automatically include all header files in the include directory and its subdirectories
file(GLOB_RECURSE HEADER_FILES "include/*.h")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(DIR ${HEADER_FILE} DIRECTORY)
    include_directories(${DIR})
endforeach()

file(GLOB_RECURSE SOURCE_FILES "lib/*.c")

add_subdirectory(src)
add_subdirectory(test)