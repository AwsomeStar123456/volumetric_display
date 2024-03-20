#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/time.h"
#include "IS31FL3731.h"

#define MTR_LEFT 0
#define MTR_RIGHT 1


void init_i2c();
void init_motor();
void set_motor_pwm(bool direction, uint8_t speed);
void stop_motor();
void person();

uint8_t frame;

/*
    This method initializes the I2C bus for pins 14 and 15 of the pico.
*/
void init_i2c()
{
    i2c_init(i2c1, 400000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
}

void init_motor()
{
    // PWM
    gpio_set_function(13, GPIO_FUNC_PWM);
    gpio_set_function(12, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(13); // Get the PWM slice for the GPIO pin
    pwm_set_enabled(slice_num, true);

    pwm_set_gpio_level(13, 0x0);
    pwm_set_gpio_level(12, 0x0);

    // High Pin for Sleep
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_put(11, 1);
}

void set_motor_pwm(bool direction, uint8_t speed) // Speed in terms of percentage
{
    // Convert speed in percentage to 16-bit value
    uint16_t reg_speed = (speed * 65535) / 100;
    
    if (direction == MTR_LEFT)
    {
        pwm_set_gpio_level(13, reg_speed);
        pwm_set_gpio_level(12, 0);
    }
    else // Right
    {
        pwm_set_gpio_level(13, 0);
        pwm_set_gpio_level(12, reg_speed);
    }
}

void stop_motor()
{
    pwm_set_gpio_level(13, 0);
    pwm_set_gpio_level(12, 0);
}

int main()
{
    stdio_init_all();
    init_i2c();
    init_ledDisplay(0);
    init_motor();

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);
    person();

    while (1) {
        // //For Person
        //  displayFrame(i2c1, ISSI_ADDR_DEFAULT, 0);
        // sleep_ms(1000);
        //  displayFrame(i2c1, ISSI_ADDR_DEFAULT, 1);
        //  sleep_ms(1000);

        set_motor_pwm(MTR_LEFT, 100);
        sleep_ms(1000);
        stop_motor();  
        sleep_ms(1000);
        set_motor_pwm(MTR_RIGHT, 100);
        sleep_ms(1000);
        stop_motor();
        sleep_ms(1000);
    }

    return 0;
}

void person(){
    frame = 0;

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 1, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 7, 100, frame);

    frame = 1;

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 1, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 7, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);

    frame = 2;

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 1, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 7, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);
}