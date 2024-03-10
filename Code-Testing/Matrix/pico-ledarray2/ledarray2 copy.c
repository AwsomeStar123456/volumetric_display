#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "pico/time.h" 

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
bool selectBank(i2c_inst_t *i2c, uint8_t addr, uint8_t bank);
bool writeRegister8(i2c_inst_t *i2c, uint8_t addr, uint8_t bank, uint8_t reg, uint8_t data);
void setLEDPWM(i2c_inst_t *i2c, uint8_t addr, uint8_t lednum, uint8_t pwm, uint8_t bank);
void displayFrame(i2c_inst_t *i2c, uint8_t addr, uint8_t* frame);
void setFrame(uint8_t* frame, uint8_t newFrame);
bool begin(i2c_inst_t *i2c, uint8_t addr, uint8_t* frame);
void audioSync(i2c_inst_t *i2c, uint8_t addr, bool sync);


void init_i2c()
{
    i2c_init(i2c1, 400000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
}

bool selectBank(i2c_inst_t *i2c, uint8_t addr, uint8_t bank) {
  uint8_t cmd[2] = {ISSI_COMMANDREGISTER, bank};
  int result = i2c_write_blocking(i2c, addr, cmd, 2, false);
  return result == PICO_ERROR_NONE;
}

bool writeRegister8(i2c_inst_t *i2c, uint8_t addr, uint8_t bank, uint8_t reg, uint8_t data) {
  if (!selectBank(i2c, addr, bank)) {
    return false;
  }

  uint8_t cmd[2] = {reg, data};
  int result = i2c_write_blocking(i2c, addr, cmd, 2, false);
  return result == PICO_ERROR_NONE;
}

void setLEDPWM(i2c_inst_t *i2c, uint8_t addr, uint8_t lednum, uint8_t pwm, uint8_t bank) {
  if (lednum >= 144)
    return;
  writeRegister8(i2c, addr, bank, 0x24 + lednum, pwm);
}

void clear(i2c_inst_t *i2c, uint8_t addr, uint8_t frame) {
  selectBank(i2c, addr, frame);
  uint8_t erasebuf[25];

  memset(erasebuf, 0, 25);

  for (uint8_t i = 0; i < 6; i++) {
    erasebuf[0] = 0x24 + i * 24;
    i2c_write_blocking(i2c, addr, erasebuf, 25, false);
  }
}

void setFrame(uint8_t* frame, uint8_t newFrame) { 
    *frame = newFrame; 
}

void displayFrame(i2c_inst_t *i2c, uint8_t addr, uint8_t* frame) {
  if (*frame > 7)
    *frame = 0;
  writeRegister8(i2c, addr, ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, *frame);
}

bool begin(i2c_inst_t *i2c, uint8_t addr, uint8_t* frame) {
  *frame = 0;

  // shutdown
  writeRegister8(i2c, addr, ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);

  sleep_ms(10);

  // out of shutdown
  writeRegister8(i2c, addr, ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);

  // picture mode
  writeRegister8(i2c, addr, ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

  displayFrame(i2c, addr, frame);

    // all LEDs on & 0 PWM
    clear(i2c, addr, *frame); // set each led to 0 PWM

    for (uint8_t f = 0; f < 8; f++) {
    for (uint8_t i = 0; i <= 0x11; i++)
        writeRegister8(i2c, addr, f, i, 0xff); // each 8 LEDs on
    }

  audioSync(i2c, addr, false);

  return true;
}

void audioSync(i2c_inst_t *i2c, uint8_t addr, bool sync) {
  if (sync) {
    writeRegister8(i2c, addr, ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1);
  } else {
    writeRegister8(i2c, addr, ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
  }
}

void swap_int16_t(int16_t* a, int16_t* b) {
    int16_t t = *a;
    *a = *b;
    *b = t;
}

void drawPixel(i2c_inst_t *i2c, uint8_t addr, int16_t x, int16_t y, uint16_t color, uint8_t rotation, uint8_t frame) {
    // check rotation, move pixel around if necessary
    switch (rotation) {
    case 1:
        swap_int16_t(&x, &y);
        x = 16 - x - 1;
        break;
    case 2:
        x = 16 - x - 1;
        y = 9 - y - 1;
        break;
    case 3:
        swap_int16_t(&x, &y);
        y = 9 - y - 1;
        break;
    }

    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;
    if (color > 255)
        color = 255; // PWM 8bit max

    setLEDPWM(i2c, addr, x + y * 16, color, frame);
}

int main()
{
    stdio_init_all();
    init_i2c();

    uint8_t frame = 0;
    uint8_t addr = ISSI_ADDR_DEFAULT;

    // Begin communication with the IS31FL3731 device
    if (!begin(i2c1, addr, &frame)) {
        printf("Failed to initialize IS31FL3731\n");
        return 1;
    }

    // Set all LEDs to maximum brightness on frame 0
    for (uint8_t led = 0; led < 144; led++) {
        setLEDPWM(i2c1, addr, led, 255, frame);
    }

    // Now turn on all LEDs
    for (uint8_t f = 0; f < 8; f++) {
        for (uint8_t i = 0; i <= 0x11; i++)
            writeRegister8(i2c1, addr, f, i, 0xff); // each 8 LEDs on
    }

    displayFrame(i2c1, addr, &frame);

    while (1) {
        // Keep the program running
    }

    return 0;
}