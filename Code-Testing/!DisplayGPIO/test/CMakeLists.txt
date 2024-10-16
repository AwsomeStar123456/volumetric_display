#--------------------------------------------------#
# This is the test_pwm executatble.
#--------------------------------------------------#

# Create the executable file.
add_executable(test_pwm
	test_pwm.c
	${ONBOARD_LED_WS2812_PATH}
	)

#Set the name and version of the project.
pico_set_program_name(test_pwm "pwm test")
pico_set_program_version(test_pwm "0.1")

#Link all of the required libraries to the project.
target_link_libraries(test_pwm PRIVATE
pico_stdlib
pico_multicore
hardware_pio
hardware_i2c
hardware_pwm
)

#Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(test_pwm)

#Generate the pio header file for the onboard LED.
pico_generate_pio_header(test_pwm ${ONBOARD_LED_PIO})

#--------------------------------------------------#
# This is the test_logging executatble.
#--------------------------------------------------#

# Create the executable file.
add_executable(test_logging
	test_logging.c
	${ONBOARD_LED_WS2812_PATH}
	)

#Set the name and version of the project.
pico_set_program_name(test_logging "log test")
pico_set_program_version(test_logging "0.1")

#Link all of the required libraries to the project.
target_link_libraries(test_logging PRIVATE
pico_stdlib
pico_multicore
hardware_pio
hardware_i2c
hardware_pwm
)

#Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(test_logging)

#Generate the pio header file for the onboard LED.
pico_generate_pio_header(test_logging ${ONBOARD_LED_PIO})
