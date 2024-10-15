# Main Binary File
add_executable(display
	display.c
	../WS2812/ws2812.c
	)

pico_set_program_name(display "display")
pico_set_program_version(display "0.1")

#Link all of the required libraries to the project.
target_link_libraries(display PRIVATE
pico_stdlib
pico_multicore
hardware_pio
hardware_i2c
hardware_pwm
)

#Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(display)

pico_generate_pio_header(display ${CMAKE_CURRENT_LIST_DIR}/../WS2812/ws2812.pio)
