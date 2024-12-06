#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#include "imagedata.h"
#include "GpioMapping.h"
#include "ws2812.pio.h"
#include "Initialization.h"
#include "Motor.h"

#define WS2812_PIN 16
#define NUM_PIXELS 1
#define IS_RGBW true

#define FLASH_TARGET_OFFSET (256 * 1024)
#define LOG_EVENT_SIZE sizeof(logevent_t)
#define NUM_LOG_EVENTS 20

const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

typedef struct {
    uint SecondsSinceBoot;
} logevent_t;

volatile bool core1_uninitialized = true;

void log_event(uint gpio, uint32_t events);

void set_onboard_led_color(PIO pio, int sm, uint32_t color);
void read_logs_from_memory();

/*
    This method sets the color of the onboard LED.
*/
void set_onboard_led_color(PIO pio, int sm, uint32_t color) {
    pio_sm_put_blocking(pio, sm, color << 8u);
}

/*
    This methods runs on cpu1 and is responsible for controlling the motor and doing the rpm detection.
*/
void core1_entry() {
    init_motor();
    init_ir_sense();

    //Set the GPIO pin 28 to trigger an interrupt on a falling edge. IR Sensor.
    //gpio_set_irq_enabled_with_callback(28, GPIO_IRQ_EDGE_FALL, true, &log_event);

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

    while (1) {

    }
}

size_t round_up_to_multiple_of_256(size_t size) {
    return (size + 255) & ~255;
}

size_t round_up_to_multiple_of_4096(size_t size) {
    return (size + 4095) & ~4095;
}

/*
    This is the main method that runs on cpu0 and is responsible for controlling the display.
*/
int main() {

    //Initialize all peripherals.
    stdio_init_all();

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

    logevent_t log_events[NUM_LOG_EVENTS];
    logevent_t log_events_read[NUM_LOG_EVENTS];
    int log_index = 0;

    // Set LED to red to indicate start
    set_onboard_led_color(pio, sm, ws2812Red);

    // Set logs
    while (log_index < NUM_LOG_EVENTS) {
        log_events[log_index].SecondsSinceBoot = to_ms_since_boot(get_absolute_time());
        log_index++;
        sleep_ms(25);
    }

    // Calculate sector size
    size_t sector_size = round_up_to_multiple_of_4096(LOG_EVENT_SIZE * NUM_LOG_EVENTS);
    size_t program_size = round_up_to_multiple_of_256(LOG_EVENT_SIZE * NUM_LOG_EVENTS);

    // Erase flash
    set_onboard_led_color(pio, sm, ws2812Blue); // Set LED to blue to indicate flash erase
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, sector_size);
    restore_interrupts(ints);

    // Write logs to flash
    set_onboard_led_color(pio, sm, ws2812Green); // Set LED to green to indicate flash write
    ints = save_and_disable_interrupts();
    flash_range_program(FLASH_TARGET_OFFSET, (const uint8_t *)log_events, program_size);
    restore_interrupts(ints);

    // Read logs from flash
    set_onboard_led_color(pio, sm, ws2812Black); // Set LED to black to indicate flash read
    const uint8_t *flash_data = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    memcpy(log_events_read, flash_data, LOG_EVENT_SIZE * NUM_LOG_EVENTS);

    // Initialize USB stdio
    stdio_usb_init();

    // Set LED to white to indicate USB initialization
    set_onboard_led_color(pio, sm, 0x00FFFFFF);
    sleep_ms(15000);

    // Print logs
    log_index = 0;
    while (log_index < NUM_LOG_EVENTS) {
        printf("Read log: %d Time since boot: %d\n", log_index, log_events_read[log_index].SecondsSinceBoot);
        log_index++;
        sleep_ms(250);
    }

    // Reset log index for the next iteration
    log_index = 0;

    // Set LED to green to indicate end
    set_onboard_led_color(pio, sm, ws2812Green);

    return 0;
}
