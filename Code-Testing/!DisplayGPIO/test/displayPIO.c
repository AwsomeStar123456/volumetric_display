#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Raspbery Pi RP2040 Libraries
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

//Our Custom Libraries
#include "imagedata.h"
#include "OriginalImageData.h"
#include "GpioMapping.h"
#include "ws2812.pio.h"
#include "Initialization.h"
#include "Motor.h"

//Base Constants
#define TARGET_RPM 1736 //Target RPM for the motor. 1736 RPM = 29 FPS (Frames Per Second)
#define TIME_UNTIL_SHUTDOWN 3000000 //Time in us = 3 Seconds | Sets the time until the motor shuts down after the last IR sensor reading.

#define PIXEL_TIME 10 //DO NOT EDIT! Time in us that each pixel is displayed for.
#define NUM_3D_FRAMES 168 //Number of 3D frames we have in the animation.
#define FRAMES_BEFORE_NEXT 1 //Number of times we repeat a frame before moving to the next 3D frame. Allows for slower FPS.

//PID Constants
#define Kp 0.05f // Proportional constant
#define Ki 0.1f // Integral constant
#define Kd 0.01f // Derivative constant
float lastError = 0.0f;
volatile int pwm = 0;

//Motor & IR Settings
//Shouldn't be touched. This part deals with the noise from the IR sensor.
#define VOLTAGE_THRESHOLD 2 // Voltage threshold in volts
#define ADC_PIN 28 // GPIO pin connected to the ADC
#define IR_SENSOR_PIN 28 // GPIO pin connected to the IR sensor
#define ADC_CHANNEL 2 // ADC channel corresponding to GPIO28
#define DEBOUNCE_TIME_MS 25 // Debounce time in milliseconds

//Onboard LED Settings
#define WS2812_PIN 16
#define NUM_PIXELS 1
#define IS_RGBW true

//Set the LED color values for easy access.
const uint32_t ws2812Red   = 0x00FF0000; // RGB format: 0x00RRGGBB
const uint32_t ws2812Green = 0x0000FF00; // RGB format: 0x00RRGGBB
const uint32_t ws2812Blue  = 0x000000FF; // RGB format: 0x00RRGGBB
const uint32_t ws2812Black = 0x00000000; // RGB format: 0x00RRGGBB

//Animation Switching Variables
#define NUM_ANIMATIONS 7 //Number of animations we have in the animation array.
int animations_currentframe = 6; //Which animation to start on.
//Animation Order: Water, Intel, Rotating Cube, Sine Wave
const uint16_t (*animations[NUM_ANIMATIONS])[24][9] = {water, intel, sinewave, bouncyball, flappybird, rain, fire}; //Array of animations.
uint16_t animations_framecount[NUM_ANIMATIONS] = {NUM_WATER_FRAMES, NUM_INTEL_FRAMES, NUM_SINEWAVE_FRAMES, NUM_BOUNCYBALL_FRAMES, NUM_FLAPPYBIRD_FRAMES, NUM_RAIN_FRAMES, NUM_FIRE_FRAMES}; //Number of frames in each animation.
uint16_t animations_framebeforenext[NUM_ANIMATIONS] = {1, 3, 1, 1, 1, 1, 1}; //Number of frames to repeat before moving to the next frame.

const uint16_t (*activeFrame)[24][9]; 

//Global Variables
volatile bool core1_uninitialized = true;
int currentRPM = 0;
bool atTargetRPM = false;
absolute_time_t lastTime, currentTime;

//Helper Methods
void set_onboard_led_color(PIO pio, int sm, uint32_t color);
float read_voltage();
void calc_rpm(uint gpio, uint32_t events);
void clearLEDs();

/*
    This method sets the color of the onboard LED.
*/
void set_onboard_led_color(PIO pio, int sm, uint32_t color) {
    pio_sm_put_blocking(pio, sm, color << 8u);
}

/*
    Returns the voltage of the ADC as a float.
*/
float read_voltage() {
    //Read 16 bit value from ADC. Convert the value to a voltage. (3.3 Reference, 12 bit ADC)
    uint16_t raw = adc_read(); 
    float voltage = raw * 3.3f / (1 << 12);
    return voltage;
}

/*
    This method calculates the RPM based on the time between pulses from the IR sensor. It
    implementes. PID, Debouncing, and PWM control for the motor.
*/
void calc_rpm(uint gpio, uint32_t events)
{
    //Ensure the interrupt is from the correct GPIO pin.
    if( gpio != IR_SENSOR_PIN ) return;

    // Check the time since the last pulse. If it is greater than the debounce time, process the pulse.
    uint64_t timeDiffus = to_us_since_boot(get_absolute_time()) - to_us_since_boot(lastTime);
    if (timeDiffus > DEBOUNCE_TIME_MS * 1000) {
        // Check if the voltage is below the threshold.
        if (read_voltage() < VOLTAGE_THRESHOLD) {
            // Save the current time.
            currentTime = get_absolute_time();        

            // Calculate the time difference in seconds and RPM.
            float timeDiffs = timeDiffus / 1000000.0f; // Convert microseconds to seconds
            currentRPM = 60.0f / timeDiffs;

            // Calculate the error and adjust PWM using PID control.
            float error = TARGET_RPM - currentRPM;
            float derivative = (error - lastError) / timeDiffs;
            float output = Kp * error + Kd * derivative;

            pwm += (int)output;

            // Clamp PWM value to a range of 40 to 100. (Motor Stalls Below 40)
            if(pwm > 100) {
                pwm = 100;
            } else if (pwm < 40) {
                pwm = 40;
            }

            set_motor_pwm(pwm);

            // Update the last time and error for the next pulse.
            lastError = error;
            lastTime = currentTime;
        }
    }
}

/*
    This methods runs on cpu1 and is responsible for controlling the motor and doing the rpm detection.
*/
void core1_entry() {
    sleep_ms(7500); //Sleep to allow user to plug in USB and open serial connection.
    printf("Initiating Core1 Setup.\n");

    //Start up the ADC.
    printf("Initiating ADC.\n");
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_CHANNEL);

    //Set up the GPIO for the IR sensor.
    printf("Initiating IR Sensor Pin.\n");
    gpio_init(28);
    gpio_set_dir(28, GPIO_IN);
    gpio_pull_up(28);

    //Set up the motor.
    printf("Initiating Motor.\n");
    init_motor();

    //Initialize the PIO and state machine. This is the PIO for the WS2812 LED.
    printf("Starting LED.\n");
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    set_onboard_led_color(pio, sm, 0x00111111); // Set LED to white

    //Report to core0 that it can start processing as core 1 has initialized its peripherals.
    core1_uninitialized = false;
    bool motorEnabled = false;
    bool motorIRQ = false;
    atTargetRPM = false;
    lastTime = get_absolute_time();

    sleep_ms(1000);

    while(true) {
        //Check if the motor should be enabled or disabled. This is based on the time since the last IR sensor reading and current system time.
        if(to_us_since_boot(get_absolute_time()) - to_us_since_boot(lastTime) > TIME_UNTIL_SHUTDOWN || to_ms_since_boot(get_absolute_time()) < 3000) {
            motorEnabled = false;

            if(read_voltage() < VOLTAGE_THRESHOLD) {
                motorEnabled = true;
            }
        } 

        //If the motor is enabled, set the PWM to 100 and start the motor IRQ.
        if(motorEnabled) {
            if(!motorIRQ) {
                set_motor_pwm(100);
                sleep_ms(2000);
                gpio_set_irq_enabled_with_callback(28, GPIO_IRQ_EDGE_FALL, true, &calc_rpm);
                sleep_ms(2000);
                motorIRQ = true;
            }

            //Set the onboard LED color based on the current RPM.
            if(currentRPM >= TARGET_RPM + 50){
                set_onboard_led_color(pio, sm, ws2812Blue);
                atTargetRPM = false;
            } else if (currentRPM <= TARGET_RPM - 50) {
                set_onboard_led_color(pio, sm, ws2812Red);
                atTargetRPM = false;
            } else {
                set_onboard_led_color(pio, sm, ws2812Green);
                atTargetRPM = true;
            }

        } else {
            //If the motor is disabled, set the PWM to 0 and disable the motor IRQ. Also set the onboard LED to black. Increment to next animation frame.
            if(motorIRQ) {
                set_motor_pwm(0);
                gpio_set_irq_enabled_with_callback(28, GPIO_IRQ_EDGE_FALL, false, NULL);
                motorIRQ = false;
                animations_currentframe++;
            }
            atTargetRPM = false;
            set_onboard_led_color(pio, sm, ws2812Black);
        }

        sleep_ms(500);
    }
}

/*
    This is the main method that runs on cpu0 and is responsible for controlling the display.
*/
int main() {

    //Initialize all peripherals.
    stdio_init_all();
    init_led();

    //Launch Core 1 & Wait for it to initialize its peripherals.
    multicore_launch_core1(core1_entry);
    while(core1_uninitialized) {}

    absolute_time_t start_time = get_absolute_time();
    absolute_time_t end_time = get_absolute_time();
    int currentVCC = 0;
    int currentGND = 0;
    bool pixelVal = 0;

    // Animation Switching Variables
    int current_animations_framebeforenext = 0;
    uint16_t animations_numframes;

    while (1) {

        if(atTargetRPM) {
            //Get the current animation frame and its respective frame count and speed.
            activeFrame = animations[animations_currentframe % NUM_ANIMATIONS];
            animations_numframes = animations_framecount[animations_currentframe % NUM_ANIMATIONS];
            current_animations_framebeforenext = animations_framebeforenext[animations_currentframe % NUM_ANIMATIONS];

            //Loop throught all of the 3D frames to create an animation.
            for(int j = 0; j < animations_numframes; j++) {

                //Slows the animation speed down by repeating a 3D frame (if desired)
                for(int count = 0; count < current_animations_framebeforenext; count++)
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
                                pixelVal = (activeFrame[j][i][y] >> x) & 1;

                                //Check GPIO pins for current pixel
                                currentVCC = gpiomap_flipped[y][16 - x][0];
                                currentGND = gpiomap_flipped[y][16 - x][1];

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
