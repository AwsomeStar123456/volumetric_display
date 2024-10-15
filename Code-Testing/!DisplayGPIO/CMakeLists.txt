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

#Include all the folders with files.
include_directories(DisplayI2C)
include_directories(ImageData)
include_directories(WS2812)

add_subdirectory(src)
