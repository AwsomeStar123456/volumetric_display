#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "IS31FL3731.h"
#include "DisplayAddons.h"

/*
    This method will set frames 0, 1, and 2 to show an animated person
    when the frames are played through.
*/
void person(){

    //Set the display to frame 0 and set the various LEDs.
    frame = 0;

    clear(i2c0, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 7, 1, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 8, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 8, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 7, 7, 100, frame);

    //Set the display to frame 1 and set the various LEDs.
    frame = 1;

    clear(i2c0, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 1, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 7, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);

    //Set the display to frame 3 and set the various LEDs.
    frame = 2;

    clear(i2c0, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 11, 1, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 10, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 10, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 11, 7, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c0, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c0, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);
}

/*
    This methods hardcodes the digits on a 3x5 grid for each number so that we
    can easily display the numbers.
*/
int digits[10][7][4] = {
    {
        {0, 0, 0, 0},  // 0
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 1
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    },
    {
        {0, 0, 0, 0},  // 2
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 3
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 4
        {0, 0, 0, 0},
        {0, 1, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1}
    },
    {
        {0, 0, 0, 0},  // 5
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 6
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 7
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    },
    {
        {0, 0, 0, 0},  // 8
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 9
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1}
    }
};

/*
    This method takes in the number to display, position from 0-3, and the frame
    for where these numbers should be written.
*/
void display_number(int number, int position, int frame) {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            if (digits[number][i][j]) {
                setPixel(i2c0, ISSI_ADDR_DEFAULT, position + j, i + 1, 100, frame);
            }
        }
    }
}

/*
    This method will take in an array of 4 numbers which it will then put on the display.
*/
void display_numbers(int numbers[4]) {
    for (int i = 0; i < 4; i++) {
        display_number(numbers[i], 4 * i, frame);
    }
}