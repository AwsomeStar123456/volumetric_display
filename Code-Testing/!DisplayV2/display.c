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
#include "IS31FL3731.h"
#include "DisplayAddons.h"
#include "imagedata.h"

#define MTR_LEFT 0
#define MTR_RIGHT 1

#define RPM_AVERAGE_COUNT 5

volatile uint32_t last_times[RPM_AVERAGE_COUNT] = {0};
volatile uint32_t rpm = 0;
volatile uint32_t rpm_index = 0;

bool enabled = false;
bool lastCleared = false;


void init_i2c();
void init_motor();
void set_motor_pwm(bool direction, uint8_t speed);
void stop_motor();
void init_ir_sense();

void calc_rpm(uint gpio, uint32_t events);
void displayImage(int *imageData);
void setPixels(int* pixelData);

volatile uint8_t frame = 0;
volatile bool core1_uninitialized = true;

int *images[] = {image_0001, image_0002, image_0003, image_0004, image_0005, 
image_0006, image_0007, image_0008, image_0009, image_0010, image_0011, 
image_0012, image_0013, image_0014, image_0015, image_0016, image_0017, 
image_0018, image_0019, image_0020, image_0021, image_0022, image_0023, image_0024};


/*
    This method initializes the I2C bus for pins 0 and 1 of the pico.
*/
void init_i2c()
{
    //Set the speed of the I2C to be 400k.
    i2c_init(i2c0, 400000);

    //Set both GPIO pins 0 and 1 to I2C mode.
    //GPIO 0 = SDA White
    //GPIO 1 = SCL Blue
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);

    //Enable the pull-up resistors for ping 0 and 1 for I2C.
    gpio_pull_up(0);
    gpio_pull_up(1);
}

/*
    This method initializes the PWM for pins 12 and 13 of the pico.
*/
void init_motor()
{
    //Set the GPIO functions of pins 13 and 12 to PWM.
    gpio_set_function(13, GPIO_FUNC_PWM);
    gpio_set_function(12, GPIO_FUNC_PWM);

    //Get the PWM slice for pin 13. This will be the same as
    //pin 12 so we just get the slice once. Set it to true.
    uint slice_num = pwm_gpio_to_slice_num(13);
    pwm_set_enabled(slice_num, true);

    //Ensure that the PWM is set to off.
    pwm_set_gpio_level(13, 0x0);
    pwm_set_gpio_level(12, 0x0);

    //Set pin 11 to high so we can use it for the enable of the motor controller board.
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_put(11, 1);
}

/*
    This method initiates the IR sensor as well as the falling edge trigger for the IR sensor.
*/
void init_ir_sense()
{
    //Initiate GPIO 26 to be an input pin so that we can read the value from the IR sensor.
    gpio_init(26);
    gpio_set_dir(26, GPIO_IN);

    //We will enable the falling edge trigger on pin 26 so that it will call our calc RPM
    //method when the GPIO detects an edge.
    gpio_set_irq_enabled_with_callback(26, GPIO_IRQ_EDGE_FALL, true, &calc_rpm);
}

/*
    This method takes in a direction and speed in percent and sets the PWM of the GPIO pins.
*/
void set_motor_pwm(bool direction, uint8_t speed)
{
    //Convert speed in percentage to 16-bit value that the motor register uses.
    uint16_t reg_speed = (speed * 65535) / 100;
    
    //Set the direction of the motor depending on the direction boolean.
    if (direction == MTR_LEFT) //Left
    {
        pwm_set_gpio_level(13, reg_speed);
        pwm_set_gpio_level(12, 0);
    }
    else //Right
    {
        pwm_set_gpio_level(13, 0);
        pwm_set_gpio_level(12, reg_speed);
    }
}

/*
    This methods stops the motor.
*/
void stop_motor()
{
    //Set the PWM levels to zero to stop the motor.
    pwm_set_gpio_level(13, 0);
    pwm_set_gpio_level(12, 0);
}

/*
    This methods runs on cpu1 and is responsible for controlling the motor and doing the rpm detection.
*/
void core1_entry() {
    //Initalize the IR sensor and the timer for RPM detection.
    init_ir_sense();

    //Report to core0 that it can start processing as core 1 has initialized its peripherals.
    core1_uninitialized = false;
    while (1) {

        if(enabled) {
            set_motor_pwm(MTR_LEFT, 100);
        } else {
            stop_motor();
        }
    }
}

/*
    This is the main method that runs on cpu0 and is responsible for controlling the display.
*/
int main()
{
    //Initialize all peripherals.
    stdio_init_all();
    init_i2c();
    init_ledDisplay(0);
    init_motor();
    clear(i2c0, ISSI_ADDR_DEFAULT, frame);
    //person();
    displayImage(images[0]);
    //Launch Core 1
    multicore_launch_core1(core1_entry);
    while(core1_uninitialized) {}

    while (1) {
    
    
    }

    return 0;
}

void displayImage(int *imageData) {
    int width = imageData[0];
    int height = imageData[1];
    int *pixels = &imageData[2];

    // Code to display the image
    for (int i = 0; i < height; i++) {
        for (int j = width - 1; j >= 0; j--) {
            setPixel(i2c0, ISSI_ADDR_DEFAULT, i, j, pixels[i * width + (width - j - 1)] * 20, frame);
        }
    }

    displayFrame(i2c0, ISSI_ADDR_DEFAULT, frame);
}

void setPixels(int* pixelData) {
    int length = pixelData[0] * 3;

    for (int i = 1; i < length; i += 3) {
        int x = pixelData[i];
        int y = pixelData[i + 1];
        int value = pixelData[i + 2];

        // Code to set the pixel
        setPixel(i2c0, ISSI_ADDR_DEFAULT, x, y, value * 100, frame);
    }

}