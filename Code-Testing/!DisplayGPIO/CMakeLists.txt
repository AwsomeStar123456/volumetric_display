#Ensure that the cmake and compiler is all correct for the pico SDK
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Set this path to match the path that your installed pico SDK is located at.
set(PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk")

include(pico_sdk_import.cmake)

#Create the display project and add all of the required files.
project(display C CXX ASM)

pico_sdk_init()

add_executable(display
	display.c
	IS31FL3731.c
	DisplayAddons.c
	)
	
#Link all of the required libraries to the project.
target_link_libraries(display pico_stdlib)
target_link_libraries(display pico_multicore)
target_link_libraries(display hardware_pio)
target_link_libraries(display hardware_i2c)
target_link_libraries(display hardware_pwm)

pico_add_extra_outputs(display)