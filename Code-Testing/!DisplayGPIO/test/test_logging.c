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

#define FLASH_TARGET_OFFSET (150 * 4096)
#define LOG_EVENT_SIZE sizeof(logevent_t)
#define NUM_LOG_EVENTS 20

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

void write_logs_to_flash(logevent_t *log_events, int num_events) {
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, LOG_EVENT_SIZE * num_events);
    flash_range_program(FLASH_TARGET_OFFSET, (const uint8_t *)log_events, LOG_EVENT_SIZE * num_events);
    restore_interrupts(ints);
}

void read_logs_from_flash(logevent_t *log_events, int num_events) {
    const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    memcpy(log_events, flash_target_contents, LOG_EVENT_SIZE * num_events);
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

    logevent_t log_events[NUM_LOG_EVENTS];
    int log_index = 0;

    while (1) {
        sleep_ms(10000);
        printf("Starting logging test\n");

        while(log_index < NUM_LOG_EVENTS) {
            log_events[log_index].SecondsSinceBoot = to_ms_since_boot(get_absolute_time());
            log_index++;
            printf("Completed log event %d\n", log_index);
            sleep_ms(250);
        }

        printf("Log Events Completed\n");

        printf("Attempting to erase set of flash. Size: %d\n", FLASH_SECTOR_SIZE);

        sleep_ms(1000);

        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
        restore_interrupts(ints);

        sleep_ms(1000);

        printf("Erased Flash. Size: %d\n", FLASH_SECTOR_SIZE);

        // Example: Read logs from flash
        logevent_t read_log_events[NUM_LOG_EVENTS];
        read_logs_from_flash(read_log_events, log_index);

        printf("Read from flash.\n");

        sleep_ms(10000);

        printf("Finished Sleep.\n");

        for (int i = 0; i < log_index; i++) {
            printf("Log %d: %u seconds since boot\n", i, read_log_events[i].SecondsSinceBoot);
        }
    }

    return 0;
}
