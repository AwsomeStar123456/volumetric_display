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
#include "Initialization.h"

#define TARGET_RPM 1736 //Target RPM for the motor
#define TIME_UNTIL_SHUTDOWN 3000000 //Time in us = 3 Seconds

#define PIXEL_TIME 10 //Time in us that each pixel is displayed for.
#define NUM_3D_FRAMES 1 //Number of 3D frames we have in the animation.
#define FRAMES_BEFORE_NEXT 1 //Number of times we repeat a frame before moving to the next 3D frame.

#define WS2812_PIN 16
#define NUM_PIXELS 1
#define IS_RGBW true

volatile bool core1_uninitialized = true;
bool atTargetRPM = false;
int currentRPM = 0;
absolute_time_t lastTimeRPM;

void clearLEDs();
void calc_rpm(uint gpio, uint32_t events);
void set_onboard_led_color(PIO pio, int sm, uint32_t color);

/*
    This method sets the color of the onboard LED.
*/
void set_onboard_led_color(PIO pio, int sm, uint32_t color) {
    pio_sm_put_blocking(pio, sm, color << 8u);
}

void calc_rpm(uint gpio, uint32_t events)
{
    absolute_time_t currentTime = get_absolute_time();

    uint64_t timeDiffus = to_us_since_boot(currentTime) - to_us_since_boot(lastTimeRPM);

    // Avoid division by zero
    if (timeDiffus > 0) {
        float timeDiffs = timeDiffus / 1000000.0f; // Convert microseconds to seconds
        float timeDiffRPM = 60.0f / timeDiffs;

        currentRPM = (int)timeDiffRPM;
    } else {
        currentRPM = 0; // Handle the case where timeDiffus is zero
    }

    lastTimeRPM = currentTime;
}

/*
    This methods runs on cpu1 and is responsible for controlling the motor and doing the rpm detection.
*/
void core1_entry() {

    lastTimeRPM = get_absolute_time();

    init_motor();
    init_ir_sense();

    //Set the GPIO pin 28 to trigger an interrupt on a falling edge. IR Sensor.
    gpio_set_irq_enabled_with_callback(28, GPIO_IRQ_EDGE_FALL, true, &calc_rpm);

    // Initialize the PIO and state machine. This is the PIO for the WS2812 LED.
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    //Set the pixel color values for easy access.
    uint32_t ws2812Red   = 0x00FF0000; // RGB format: 0x00RRGGBB
    uint32_t ws2812Green = 0x0000FF00; // RGB format: 0x00RRGGBB
    uint32_t ws2812Blue  = 0x000000FF; // RGB format: 0x00RRGGBB
    uint32_t ws2812Black = 0x00000000; // RGB format: 0x00RRGGBB

    uint32_t ws2812Color = 0x00000000; // RGB format: 0x00RRGGBB

    set_onboard_led_color(pio, sm, ws2812Black);

    //Report to core0 that it can start processing as core 1 has initialized its peripherals.
    core1_uninitialized = false;
    bool motorEnabled = false;

    absolute_time_t whileLoopBuffer = get_absolute_time();

    //Variables for PWM using PID
    double pwm = 50;

    double Kp = 0.01;

    double error = 0;

    while (1) {
        whileLoopBuffer = get_absolute_time();

        if(motorEnabled) {   
            error = TARGET_RPM - currentRPM;

            pwm += Kp * error;

            if(pwm > 100) pwm = 100;
            if(pwm < 0) pwm = 0;

            set_motor_pwm((uint8_t)pwm);

            //If RPM is valid we need to enable atTargetRPM boolean
            if(currentRPM > TARGET_RPM - 50 && currentRPM < TARGET_RPM + 50) {
                atTargetRPM = true;
            } else {
                atTargetRPM = false;
            }

            // if (currentRPM < TARGET_RPM - 50) {
            //      set_onboard_led_color(pio, sm, ws2812Red);
            // } else if (currentRPM > TARGET_RPM + 50) {
            //      set_onboard_led_color(pio, sm, ws2812Blue);
            // } else {
            //     set_onboard_led_color(pio, sm, ws2812Green);
            // }

            // RGB format: 0x00RRGGBB
            if(pwm >= 0 && pwm < 80)    ws2812Color = 0x00FF0000; //Red
            if(pwm >= 80 && pwm < 85)   ws2812Color = 0x00FFFF00; //Yellow
            if(pwm >= 85 && pwm < 90)   ws2812Color = 0x0000FF00; //Green
            if(pwm >= 90 && pwm < 95)   ws2812Color = 0x0000FFFF; //Cyan
            if(pwm >= 95 && pwm <= 100) ws2812Color = 0x000000FF; //Blue

            set_onboard_led_color(pio, sm, ws2812Color);

        } else {
            set_onboard_led_color(pio, sm, 0x00FF00A0);

            stop_motor();
        }

        //If 3 seconds past the last RPM reading, we need to disable atTargetRPM boolean
        if((to_us_since_boot(get_absolute_time()) - to_us_since_boot(lastTimeRPM) > TIME_UNTIL_SHUTDOWN) || to_ms_since_boot(get_absolute_time()) < 3000) {
            motorEnabled = false;
            atTargetRPM = false;
            pwm = 50;
        } else {
            motorEnabled = true;
        }

        while(to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(whileLoopBuffer) < 100) {}

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
        //atTargetRPM
        if(true) {

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