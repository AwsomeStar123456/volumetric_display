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

#define ISSI_ADDR_DEFAULT 0x74

#define ISSI_REG_CONFIG 0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME 0x01

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B // helpfully called 'page nine'


void init_i2c();
void person();
void sweepPixels();

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

int main()
{
    stdio_init_all();
    init_i2c();
    init_ledDisplay(0);

    //selectBank(i2c1, ISSI_ADDR_DEFAULT, frame);

    //displayFrame(i2c1, ISSI_ADDR_DEFAULT, frame);

    //setAllLEDPWM(i2c1, ISSI_ADDR_DEFAULT, 255, frame);

    //setPixel(i2c1, ISSI_ADDR_DEFAULT, 15, 8, 255, frame);
    //sleep_ms(1000);

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);
    person();

    //int i = 0;
    while (1) {
        // for (uint8_t start = 0; start < 144; start += 16) {
        //     // Clear all LEDs
        //     clear(i2c1, ISSI_ADDR_DEFAULT, frame);

        //     // Set the LEDs in the current line
        //     for (uint8_t led = start; led < start + 16 && led < 144; led++) {
        //         setLEDPWM(i2c1, ISSI_ADDR_DEFAULT, led, 255, frame);
        //     }

        //     // Wait for 1 second
        //     sleep_ms(1000);
        // }

        // clear(i2c1, ISSI_ADDR_DEFAULT, frame);
        // sweepPixels();
        // clear(i2c1, ISSI_ADDR_DEFAULT, frame);

        //For Person
         displayFrame(i2c1, ISSI_ADDR_DEFAULT, 0);
        sleep_ms(1000);
         displayFrame(i2c1, ISSI_ADDR_DEFAULT, 1);
         sleep_ms(1000);
         //displayFrame(i2c1, ISSI_ADDR_DEFAULT, 2);
        //sleep_ms(1000);
    }

    return 0;
}

void sweepPixels() {
    // Iterate over all rows
    for (uint8_t row = 0; row < 16; row++) {
        // Iterate over all columns
        for (uint8_t col = 0; col < 9; col++) {
            // Light up the current pixel
            setPixel(i2c1, ISSI_ADDR_DEFAULT, row, col, 20, frame);
            sleep_ms(2);
        }
    }
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