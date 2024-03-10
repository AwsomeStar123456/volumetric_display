#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "pico/time.h"
#include "IS31FL3731.h"
#include "imagedata.h"

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
#define ISSI_BANK_FUNCTIONREG 0x0B // page nine

#define RPM 60 


void init_i2c();
void person();
void sweepPixels();
void displayImage(int *imageData);

uint8_t frame;

int *images[] = {image_0001, image_0002, image_0003, image_0004, image_0005, 
image_0006, image_0007, image_0008, image_0009, image_0010, image_0011, 
image_0012, image_0013, image_0014, image_0015, image_0016, image_0017, 
image_0018, image_0019, image_0020, image_0021, image_0022, image_0023, image_0024};

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

// Function to display image
void displayImage(int *imageData) {
    int width = imageData[0];
    int height = imageData[1];
    int *pixels = &imageData[2];

    // Code to display the image
    for (int i = 0; i < height; i++) {
        for (int j = width - 1; j >= 0; j--) {
            setPixel(i2c1, ISSI_ADDR_DEFAULT, i, j, pixels[i * width + (width - j - 1)] * 20, frame);
        }
    }

    displayFrame(i2c1, ISSI_ADDR_DEFAULT, frame);
}

int main()
{
    stdio_init_all();
    init_i2c();
    init_ledDisplay(0);

    frame = 0;

    //displayImage(image_0020);

    int delay_time = (60 * 1000) / (RPM * 24); // 60 seconds * 1000 ms/sec divided by (RPM * number of images)

    while (1) {
        // Loop through each image
        for (int i = 0; i < 24; i++) {
            displayImage(images[i]);
            sleep_ms(delay_time); // Delay between images, adjust as needed
        }
    }
}