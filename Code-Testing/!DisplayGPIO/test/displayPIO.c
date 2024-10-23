#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "pico/sync.h"
#include "pico/platform.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#include "imagedata.h"
#include "GpioMapping.h"
#include "ws2812.pio.h"
#include "Initialization.h"
#include "Motor.h"


#define TARGET_RPM 1200 //Target RPM for the motor
#define TIME_UNTIL_SHUTDOWN 3000000 //Time in us = 3 Seconds

#define PIXEL_TIME 10 //Time in us that each pixel is displayed for.
#define NUM_3D_FRAMES 1 //Number of 3D frames we have in the animation.
#define FRAMES_BEFORE_NEXT 1 //Number of times we repeat a frame before moving to the next 3D frame.

#define VOLTAGE_THRESHOLD 1.2 // Voltage threshold in volts
#define ADC_PIN 28 // GPIO pin connected to the ADC
#define ADC_CHANNEL 2 // ADC channel corresponding to GPIO28


volatile int pulse_count = 0;
volatile bool previous_below_threshold = false;

#define WS2812_PIN 16
#define NUM_PIXELS 1
#define IS_RGBW true

//Set the pixel color values for easy access.
const uint32_t ws2812Red   = 0x00FF0000; // RGB format: 0x00RRGGBB
const uint32_t ws2812Green = 0x0000FF00; // RGB format: 0x00RRGGBB
const uint32_t ws2812Blue  = 0x000000FF; // RGB format: 0x00RRGGBB
const uint32_t ws2812Black = 0x00000000; // RGB format: 0x00RRGGBB

volatile bool core1_uninitialized = true;
bool atTargetRPM = false;
int currentRPM = 0;
absolute_time_t lastTime, currentTime;
bool is_captured;

void clearLEDs();
void calc_rpm(uint gpio, uint32_t events);
void set_onboard_led_color(PIO pio, int sm, uint32_t color);

/*
    This method sets the color of the onboard LED.
*/
void set_onboard_led_color(PIO pio, int sm, uint32_t color) {
    pio_sm_put_blocking(pio, sm, color << 8u);
}

float read_voltage() {
    uint16_t raw = adc_read();
    float voltage = raw * 3.7f / (1 << 12); // Convert raw value to voltage
    return voltage;
}

/*
    This methods runs on cpu1 and is responsible for controlling the motor and doing the rpm detection.
*/
void core1_entry() {
    sleep_ms(7500); //Sleep to allow user to plug in USB and open serial connection.
    printf("Initiating Core1 Setup.\n");

    printf("Initiating ADC.\n");
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_CHANNEL);

    printf("Initiating IR Sensor Pin.\n");
    gpio_init(28);
    gpio_set_dir(28, GPIO_IN);
    gpio_pull_up(28);

    printf("Initiating Motor.\n");
    init_motor();

    //Initiate pulse_count so we know how many ticks have happened.
    pulse_count = 0;

    // Initialize the PIO and state machine. This is the PIO for the WS2812 LED.
    printf("Starting LED.\n");
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    set_onboard_led_color(pio, sm, 0x00111111); // Set LED to white

    // Report to core0 that it can start processing as core 1 has initialized its peripherals.
    core1_uninitialized = false;
    bool motorEnabled = false;

    uint32_t ws2812Color = 0x00111111; // RGB format: 0x00RRGGBB

    uint16_t pwm = 100;
    set_motor_pwm(pwm);
    sleep_ms(1000); // Allow time for the motor to start

    float Kp = 0.2; // Proportional gain

    //Log The Current Time
    uint64_t timeBetweenUS = 0;
    lastTime = get_absolute_time();
    currentTime = get_absolute_time();

    absolute_time_t verification_time = get_absolute_time(); //For verification.
    uint64_t timeBetween = 0;
    bool isRed = true;

    // Calculate the expected time interval for one revolution based on the target RPM
    uint64_t expectedTimeUS = 60000000 / TARGET_RPM; // 60 seconds in microseconds divided by target RPM


    while (true) {

        while(read_voltage() > VOLTAGE_THRESHOLD);
        currentTime = get_absolute_time();
        timeBetweenUS = to_us_since_boot(currentTime) - to_us_since_boot(lastTime);
        lastTime = currentTime;

        // Calculate the error based on the expected time interval
        int error = expectedTimeUS - timeBetweenUS;

        // Proportional control
        int pwm = (int)(Kp * error);

        if (pwm > 100) {
            pwm = 100;
        } else if (pwm < 50) {
            pwm = 50;
        }

        set_motor_pwm(pwm);

        if(isRed) {
            set_onboard_led_color(pio, sm, ws2812Red);
            isRed = false;
        } else {
            set_onboard_led_color(pio, sm, ws2812Green);
            isRed = true;
        }

        // if(currentRPM >= TARGET_RPM + 50) {
        //     set_onboard_led_color(pio, sm, ws2812Blue);
        // } else if (currentRPM <= TARGET_RPM - 50) {
        //     set_onboard_led_color(pio, sm, ws2812Red);
        // } else {
        //     set_onboard_led_color(pio, sm, ws2812Green);
        // }

        // verification_time = get_absolute_time();
        // timeBetween = to_us_since_boot(verification_time) - to_us_since_boot(currentTime);
        //printf("Time From Start to Finish: %d\n", timeBetween);
        // printf("Current RPM: %d\n", currentRPM);
        // printf("Current PWM: %d\n", pwm);
        // printf("Current Err: %d\n", error);
        // printf("expecTime: %llu\n", expectedTimeUS);
        // printf("Time Between US: %llu\n", timeBetweenUS);
        sleep_ms(30);
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

                                //Wait until PIXEL_TIME (microseconds) has passed since the start time to ensure timing
                                while (to_us_since_boot(get_absolute_time()) - to_us_since_boot(start_time) < PIXEL_TIME){}

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