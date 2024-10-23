#--------------------------------------------------#
# This is the test_pwm executatble.
#--------------------------------------------------#

# Create the executable file.
add_executable(test_pwm
	test_pwm.c
	${ONBOARD_LED_WS2812_PATH}
	${SOURCE_FILES}
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
hardware_dma
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
    ${SOURCE_FILES}
)

# Set the name and version of the project.
pico_set_program_name(test_logging "log test")
pico_set_program_version(test_logging "0.1")

# Link all of the required libraries to the project.
target_link_libraries(test_logging PRIVATE
    pico_stdlib
    pico_stdio_usb
    pico_multicore
    hardware_pio
    hardware_i2c
    hardware_pwm
    hardware_flash
    hardware_sync
    hardware_dma
)

# Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(test_logging)

# Generate the pio header file for the onboard LED.
pico_generate_pio_header(test_logging ${ONBOARD_LED_PIO})

#--------------------------------------------------#
# This is the test_flashingexample executatble.
#--------------------------------------------------#

# Create the executable file.
add_executable(test_flashingexample
	test_flashingexample.c
	${ONBOARD_LED_WS2812_PATH}
	${SOURCE_FILES}
	)

#Set the name and version of the project.
pico_set_program_name(test_flashingexample "pwm test")
pico_set_program_version(test_flashingexample "0.1")

#Link all of the required libraries to the project.
target_link_libraries(test_flashingexample PRIVATE
pico_stdlib
pico_stdio_usb
pico_multicore
hardware_pio
hardware_i2c
hardware_pwm
hardware_flash
hardware_sync
hardware_dma
)

#Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(test_flashingexample)

#Generate the pio header file for the onboard LED.
pico_generate_pio_header(test_flashingexample ${ONBOARD_LED_PIO})

#--------------------------------------------------#
# This is the displayPIO executatble.
#--------------------------------------------------#

# Create the executable file.
add_executable(displayPIO
displayPIO.c
    ${ONBOARD_LED_WS2812_PATH}
    ${SOURCE_FILES}
)

# Set the name and version of the project.
pico_set_program_name(displayPIO "log test")
pico_set_program_version(displayPIO "0.1")

# Link all of the required libraries to the project.
target_link_libraries(displayPIO PRIVATE
    pico_stdlib
    pico_stdio_usb
    pico_multicore
    hardware_pio
    hardware_i2c
    hardware_pwm
    hardware_flash
    hardware_sync
    hardware_irq
    hardware_clocks
    hardware_dma
    hardware_adc
    hardware_gpio
    hardware_timer
)

# Add the extra outputs to the project. This creates a .uf2 file that can be flashed to the pico.
pico_add_extra_outputs(displayPIO)

pico_enable_stdio_usb(displayPIO 1)
pico_enable_stdio_uart(displayPIO 0)

# Generate the pio header file for the onboard LED.
pico_generate_pio_header(displayPIO ${ONBOARD_LED_PIO})