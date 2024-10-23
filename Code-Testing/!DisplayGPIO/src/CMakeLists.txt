#--------------------------------------------------#
# This is the display executatble.
#--------------------------------------------------#

# Create the executable file.
add_executable(display
	display.c
	${ONBOARD_LED_WS2812_PATH}
	${SOURCE_FILES}
	)

#Set the name and version of the project.
pico_set_program_name(display "display")
pico_set_program_version(display "0.1")

#Link all of the required libraries to the project.
target_link_libraries(display PRIVATE
pico_stdlib
pico_multicore
hardware_pio
hardware_i2c
hardware_pwm
hardware_dma
)

#Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(display)

pico_generate_pio_header(display ${ONBOARD_LED_PIO})
