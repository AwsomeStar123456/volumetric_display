#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "imagedata.h"
#include "GpioMapping.h"
#include "ws2812.pio.h"

#define TARGET_RPM 1200 //Target RPM for the motor
#define TIME_UNTIL_SHUTDOWN 3000000 //Time in us = 3 Seconds

#define PIXEL_TIME 10 //Time in us that each pixel is displayed for.
#define NUM_3D_FRAMES 1 //Number of 3D frames we have in the animation.
#define FRAMES_BEFORE_NEXT 1 //Number of times we repeat a frame before moving to the next 3D frame.

#define WS2812_PIN 16
#define NUM_PIXELS 1
#define IS_RGBW true

volatile bool core1_uninitialized = true;
bool atTargetRPM = false;
int currentRPM = 75;
absolute_time_t lastTimeRPM;

void clearLEDs();

void init_ir_sense();
void init_motor();
void stop_motor();
void set_motor_pwm(uint8_t speed);
void calc_rpm(uint gpio, uint32_t events);

void set_onboard_led_color(PIO pio, int sm, uint32_t color);


void init_led()
{
    gpio_set_function(0, GPIO_FUNC_SIO);
    gpio_set_function(1, GPIO_FUNC_SIO);
    gpio_set_function(2, GPIO_FUNC_SIO);
    gpio_set_function(3, GPIO_FUNC_SIO);
    gpio_set_function(4, GPIO_FUNC_SIO);
    gpio_set_function(5, GPIO_FUNC_SIO);
    gpio_set_function(6, GPIO_FUNC_SIO);
    gpio_set_function(7, GPIO_FUNC_SIO);
    gpio_set_function(8, GPIO_FUNC_SIO);
    gpio_set_function(9, GPIO_FUNC_SIO);
    gpio_set_function(10, GPIO_FUNC_SIO);
    gpio_set_function(11, GPIO_FUNC_SIO);
    gpio_set_function(12, GPIO_FUNC_SIO);
    gpio_set_function(13, GPIO_FUNC_SIO);
    gpio_set_function(14, GPIO_FUNC_SIO);
    gpio_set_function(15, GPIO_FUNC_SIO);
    gpio_set_function(26, GPIO_FUNC_SIO);
    gpio_set_function(27, GPIO_FUNC_SIO);

    gpio_disable_pulls(0);
    gpio_disable_pulls(1);
    gpio_disable_pulls(2);
    gpio_disable_pulls(3);
    gpio_disable_pulls(4);
    gpio_disable_pulls(5);
    gpio_disable_pulls(6);
    gpio_disable_pulls(7);
    gpio_disable_pulls(8);
    gpio_disable_pulls(9);
    gpio_disable_pulls(10);
    gpio_disable_pulls(11);
    gpio_disable_pulls(12);
    gpio_disable_pulls(13);
    gpio_disable_pulls(14);
    gpio_disable_pulls(15);
    gpio_disable_pulls(26);
    gpio_disable_pulls(27);


    gpio_set_dir(0, GPIO_IN);
    gpio_set_dir(1, GPIO_IN);
    gpio_set_dir(2, GPIO_IN);
    gpio_set_dir(3, GPIO_IN);
    gpio_set_dir(4, GPIO_IN);
    gpio_set_dir(5, GPIO_IN);
    gpio_set_dir(6, GPIO_IN);
    gpio_set_dir(7, GPIO_IN);
    gpio_set_dir(8, GPIO_IN);
    gpio_set_dir(9, GPIO_IN);
    gpio_set_dir(10, GPIO_IN);
    gpio_set_dir(11, GPIO_IN);
    gpio_set_dir(12, GPIO_IN);
    gpio_set_dir(13, GPIO_IN);
    gpio_set_dir(14, GPIO_IN);
    gpio_set_dir(15, GPIO_IN);
    gpio_set_dir(26, GPIO_IN);
    gpio_set_dir(27, GPIO_IN);
    
}

/*
    This method sets the color of the onboard LED.
*/
void set_onboard_led_color(PIO pio, int sm, uint32_t color) {
    pio_sm_put_blocking(pio, sm, color << 8u);
}

/*
    This method initializes the PWM for pins 12 and 13 of the pico.
*/
void init_motor()
{
    //Set the GPIO functions of pins 13 and 12 to PWM.
    gpio_set_function(29, GPIO_FUNC_PWM);

    //Get the PWM slice for pin 13. This will be the same as
    //pin 12 so we just get the slice once. Set it to true.
    uint slice_num = pwm_gpio_to_slice_num(29);
    pwm_set_enabled(slice_num, true);


    //Ensure that the PWM is set to off.
    pwm_set_gpio_level(29, 0x0);
}

/*
    This method initiates the IR sensor as well as the falling edge trigger for the IR sensor.
*/
void init_ir_sense()
{
    //Initiate GPIO 26 to be an input pin so that we can read the value from the IR sensor.
    gpio_init(28);
    gpio_set_dir(28, GPIO_IN);

    //We will enable the falling edge trigger on pin 26 so that it will call our calc RPM
    //method when the GPIO detects an edge.
    gpio_set_irq_enabled_with_callback(28, GPIO_IRQ_EDGE_FALL, true, &calc_rpm);
}

void calc_rpm(uint gpio, uint32_t events)
{
    absolute_time_t currentTime = get_absolute_time();

    uint64_t timeDiffus = to_us_since_boot(currentTime) - to_us_since_boot(lastTimeRPM);
    uint64_t timeDiffms = timeDiffus / 1000;
    float timeDiffs = timeDiffms / 1000.0f;
    float timeDiffRPM = 60.0f / timeDiffs;

    currentRPM = timeDiffRPM;
    lastTimeRPM = currentTime;
}

/*
    This method takes in a direction and speed in percent and sets the PWM of the GPIO pins.
*/
void set_motor_pwm(uint8_t speed)
{
    //Convert speed in percentage to 16-bit value that the motor register uses.
    uint16_t reg_speed = (speed * 65535) / 100;

    pwm_set_gpio_level(29, reg_speed);
}

/*zz

    This methods stops the motor.
*/
void stop_motor()
{
    //Set the PWM levels to zero to stop the motor.
    pwm_set_gpio_level(29, 0);
}

/*
    This methods runs on cpu1 and is responsible for controlling the motor and doing the rpm detection.
*/
void core1_entry() {

    lastTimeRPM = get_absolute_time();

    init_motor();
    init_ir_sense();

    // Initialize the PIO and state machine. This is the PIO for the WS2812 LED.
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    //Set the pixel color values for easy access.
    uint32_t ws2812Red   = 0x00FF0000; // RGB format: 0x00RRGGBB
    uint32_t ws2812Green = 0x0000FF00; // RGB format: 0x00RRGGBB
    uint32_t ws2812Blue  = 0x000000FF; // RGB format: 0x00RRGGBB

    set_onboard_led_color(pio, sm, ws2812Blue);

    //Report to core0 that it can start processing as core 1 has initialized its peripherals.
    core1_uninitialized = false;
    bool motorEnabled = false;

    while (1) {
        if(motorEnabled) {   
            //Check Current RPM and change PWM based on the current RPM
            if(currentRPM < TARGET_RPM) {
                set_motor_pwm(60);
            } else {
                set_motor_pwm(40);
            }
            //If RPM is valid we need to enable atTargetRPM boolean
            if(currentRPM > TARGET_RPM - 50 && currentRPM < TARGET_RPM + 50) {
                atTargetRPM = true;
            } else {
                atTargetRPM = false;
            }
        } else {
            stop_motor();
        }

        //If 3 seconds past the last RPM reading, we need to disable atTargetRPM boolean
        if(to_us_since_boot(get_absolute_time()) - to_us_since_boot(lastTimeRPM) > TIME_UNTIL_SHUTDOWN) {
            motorEnabled = false;
            atTargetRPM = false;
        } else {
            motorEnabled = true;
        }

    }
}

/*
    This is the main method that runs on cpu0 and is responsible for controlling the display.
*/
int main() {

    //Initialize all peripherals.
    stdio_init_all();
    init_led();

    //Launch Core 1
    multicore_launch_core1(core1_entry);
    while(core1_uninitialized) {}

    absolute_time_t start_time = get_absolute_time();
    absolute_time_t end_time = get_absolute_time();
    int currentVCC = 0;
    int currentGND = 0;
    bool pixelVal = 0;

    while (1) {

        // TODO - Replace true with atTargetRPM
        if(atTargetRPM) {

            //Loop throught all of the 3D frames to create an animation.
            for(int j = 0; j < NUM_3D_FRAMES; j++) {

                int count = 0;
                while(count < FRAMES_BEFORE_NEXT)
                {
                    //Loop through all 24 2D slices of the 3D frame
                    for(int i = 0; i < 24; i++) {

                        //Loop through all pixels in the slice
                        for(int y = 0; y < 9; y++) {
                            for(int x = 0; x < 16; x++) {

                                //Start Timing
                                start_time = get_absolute_time();

                                //Disable previous pins and move to next pixel
                                gpio_set_dir(currentVCC, GPIO_IN);
                                gpio_set_dir(currentGND, GPIO_IN);

                                //Get Pixel On Off Status
                                pixelVal = (image_data[j][i][y] >> x) & 1;

                                //Check GPIO pins for current pixel
                                currentVCC = gpiomap_flipped[y][x][0];
                                currentGND = gpiomap_flipped[y][x][1];

                                //If the pixel should be on set its GPIO pins respectively
                                if(pixelVal == 1) {
                                    gpio_set_dir(currentVCC, GPIO_OUT);
                                    gpio_set_dir(currentGND, GPIO_OUT);
                                    gpio_put(currentVCC, 1);
                                    gpio_put(currentGND, 0);
                                    
                                }

                                //end_time = get_absolute_time();

                                //Wait until PIXEL_TIME (microseconds) has passed since the start time to ensure timing
                                while (to_us_since_boot(get_absolute_time()) - to_us_since_boot(start_time) < PIXEL_TIME){}

                                // if(to_us_since_boot(end_time) - to_us_since_boot(start_time) > 9) {
                                //     gpio_set_dir(currentVCC, GPIO_OUT);
                                //     gpio_set_dir(currentGND, GPIO_OUT);

                                //     while(true) {}
                                // }

                            }
                        }
                    }
                    count++;
                }
            }
        } else {
            clearLEDs();
        }


    }

    return 0;
}

void clearLEDs() {

    for(int y = 0; y < 9; y++) {
        for(int x = 0; x < 16; x++) {
    
            //Check GPIO pins for current pixel
            int currentVCC = gpiomap_flipped[y][x][0];
            int currentGND = gpiomap_flipped[y][x][1];

            //Disable pins and move to next pixel
            gpio_set_dir(currentVCC, GPIO_IN);
            gpio_set_dir(currentGND, GPIO_IN);
        }
    }
}