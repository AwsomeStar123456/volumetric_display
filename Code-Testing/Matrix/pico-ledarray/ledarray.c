#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "pico/time.h" 

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define IS31FL3731_ADDRESS 0x74
#define IS31FL3731_COMMANDREGISTER 0xFD
#define IS31FL3731_BANK_FUNCTIONREG 0x0B
#define IS31FL3731_REG_SHUTDOWN 0x0A
#define IS31FL3731_REG_PICTUREFRAME 0x01
#define IS31FL3731_REG_CONFIG 0x00
#define IS31FL3731_REG_CONFIG_PICTUREMODE 0x00
#define IS31FL3731_REG_AUDIOSYNC 0x06

void my_i2c_init() {
    i2c_init(I2C_PORT, 400 * 1000); 
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void i2c_send(uint8_t register_address, uint8_t data) {
    uint8_t buffer[2];
    buffer[0] = register_address;
    buffer[1] = data;
    i2c_write_blocking(I2C_PORT, IS31FL3731_ADDRESS, buffer, 2, false);
}

uint8_t i2c_receive(uint8_t register_address) {
    uint8_t data;
    i2c_write_blocking(I2C_PORT, IS31FL3731_ADDRESS, &register_address, 1, true); // Send the register address
    i2c_read_blocking(I2C_PORT, IS31FL3731_ADDRESS, &data, 1, false); // Read the data
    return data;
}

void is31fl3731_init() {
    // Select function register bank
    i2c_send(IS31FL3731_COMMANDREGISTER, IS31FL3731_BANK_FUNCTIONREG);
    // Turn off software shutdown
    i2c_send(IS31FL3731_REG_SHUTDOWN, 0x01);
    // Turn off audio sync
    i2c_send(IS31FL3731_REG_AUDIOSYNC, 0x00);
    // Select picture mode
    i2c_send(IS31FL3731_REG_CONFIG, IS31FL3731_REG_CONFIG_PICTUREMODE);
    // Select frame 0
    i2c_send(IS31FL3731_REG_PICTUREFRAME, 0x00);

    // Turn off blink for all LEDs and turn on all LEDs
    for (uint8_t lednum = 0; lednum < 144; lednum++) {
        uint8_t page = 0; // LED Control and Blink Control registers are in page 0
        uint8_t address_led = lednum / 8; // LED Control registers start at address 0x00 and each register controls 8 LEDs
        uint8_t address_blink = 0x12 + lednum / 8; // Blink Control registers start at address 0x12 and each register controls 8 LEDs
        uint8_t bit = lednum % 8; // Each bit in the register controls one LED

        i2c_send(IS31FL3731_COMMANDREGISTER, page);
        uint8_t current_state = i2c_receive(address_led); // Read the current state of the LED Control register
        current_state |= (1 << bit); // Turn on the LED
        i2c_send(address_led, current_state); // Write the new state back to the LED Control register

        current_state = i2c_receive(address_blink); // Read the current state of the Blink Control register
        current_state &= ~(1 << bit); // Turn off blink for the LED
        i2c_send(address_blink, current_state); // Write the new state back to the Blink Control register

        sleep_ms(1);
    }
}


void setLEDPWM(uint8_t lednum, uint8_t pwm) {
    if (lednum >= 144) return;

    uint8_t page = 0; // PWM registers are in page 0
    uint8_t address = 0x24 + (lednum / 16) * 0x10 + (lednum % 16); // PWM registers start at address 0x24 and are spaced 10h apart for each row

    i2c_send(IS31FL3731_COMMANDREGISTER, page);
    i2c_send(address, pwm);
}

void clear() {
    for (uint8_t lednum = 0; lednum < 144; lednum++) {
        setLEDPWM(lednum, 50);
        sleep_ms(1);
    }
}

int main()
{
    stdio_init_all();

    my_i2c_init();
    is31fl3731_init();

    clear(); // Clear all LEDs

    while (1)
    {

    }
}