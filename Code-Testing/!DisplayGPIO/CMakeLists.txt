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

add_executable(display
	display.c
	WS2812/ws2812.c
	)
	
#Link all of the required libraries to the project.
target_link_libraries(display pico_stdlib)
target_link_libraries(display pico_multicore)
target_link_libraries(display hardware_pio)
target_link_libraries(display hardware_i2c)
target_link_libraries(display hardware_pwm)

#Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(display)

pico_generate_pio_header(display ${CMAKE_CURRENT_LIST_DIR}/WS2812/ws2812.pio)