cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk")

include(pico_sdk_import.cmake)

project(TestDisplay C CXX ASM)

pico_sdk_init()

add_executable(TestDisplay
	TestDisplay.c
	)
	
target_link_libraries(TestDisplay pico_stdlib)
target_link_libraries(TestDisplay hardware_pio)
target_link_libraries(TestDisplay hardware_i2c)

pico_add_extra_outputs(TestDisplay)