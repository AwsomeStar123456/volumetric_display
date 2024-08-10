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
#include "ws2812.pio.h"

#define WS2812_PIN 16
#define NUM_PIXELS 1
#define IS_RGBW true

void set_pixel_color(PIO pio, int sm, uint32_t color) {
    pio_sm_put_blocking(pio, sm, color << 8u);
}


volatile bool core1_uninitialized = true;

/*
    This methods runs on cpu1 and is responsible for controlling the motor and doing the rpm detection.
*/
void core1_entry() {

    //Report to core0 that it can start processing as core 1 has initialized its peripherals.
    core1_uninitialized = false;

    while (1) {

    }
}

/*
    This is the main method that runs on cpu0 and is responsible for controlling the display.
*/
int main() {

    //Initialize all peripherals.
    stdio_init_all();

    //Launch Core 1
    multicore_launch_core1(core1_entry);
    while(core1_uninitialized) {}

    // Initialize the PIO and state machine
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Set the pixel color to red
    uint32_t red_color = 0x00FFFFFF; // RGB format: 0x00RRGGBB
    set_pixel_color(pio, sm, red_color);


    while (1) {
        sleep_ms(1000);
    }

    return 0;
}