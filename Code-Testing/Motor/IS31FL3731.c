#include "IS31FL3731.h"
#include <string.h>

/*
    This method initializes the LED display.
*/
void init_ledDisplay(uint8_t frame)
{
    //Enable the software shutdown for 10ms then disable it.
    writeRegister8(i2c1, ISSI_ADDR_DEFAULT, ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);
    sleep_ms(10);
    writeRegister8(i2c1, ISSI_ADDR_DEFAULT, ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);

    //Set the display to picture mode.
    writeRegister8(i2c1, ISSI_ADDR_DEFAULT, ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

    //Ensure we are displaying the current frame.
    displayFrame(i2c1, ISSI_ADDR_DEFAULT, frame);

    //Clear the current frame.
    clear(i2c1, ISSI_ADDR_DEFAULT, frame);

    //Enable all of the LEDs on the current frame.
    for (uint8_t f = 0; f < 8; f++) {
        for (uint8_t i = 0; i <= 0x11; i++)
         writeRegister8(i2c1, ISSI_ADDR_DEFAULT, f, i, 0xff); // each 8 LEDs on
    }

    //Turn off audio mode.
    writeRegister8(i2c1, ISSI_ADDR_DEFAULT, ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
}

/*
    This method selects the bank (aka frame) to write to.
*/
bool selectBank(i2c_inst_t *i2c, uint8_t addr, uint8_t bank) {
    uint8_t cmd[2] = {ISSI_COMMANDREGISTER, bank};
    int result = i2c_write_blocking(i2c, addr, cmd, 2, false);

    return result == PICO_ERROR_GENERIC ? false : true;
}

/*
    This method writes a byte to a register on the LED display based on the frame.
*/
bool writeRegister8(i2c_inst_t *i2c, uint8_t addr, uint8_t bank, uint8_t reg, uint8_t data) {
    selectBank(i2c, addr, bank);

    uint8_t cmd[2] = {reg, data};
    int result = i2c_write_blocking(i2c, addr, cmd, 2, false);

    return result == PICO_ERROR_GENERIC ? false : true;
}

/*
    This method displays the frame on the LED display (0-8).
*/
void displayFrame(i2c_inst_t *i2c, uint8_t addr, uint8_t frame) {
    if (frame > 7)
        frame = 0;
    writeRegister8(i2c, addr, ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, frame);
}

/*
    This method clears the frame on the LED display (0-8).
*/
void clear(i2c_inst_t *i2c, uint8_t addr, uint8_t frame) {
    selectBank(i2c, addr, frame);
    uint8_t erasebuf[25];

    memset(erasebuf, 0, 25);

    for (uint8_t i = 0; i < 6; i++) {
        erasebuf[0] = 0x24 + i * 24;
        i2c_write_blocking(i2c, addr, erasebuf, 25, false);
    }
}

/*
    This method sets the PWM for a specific LED on the LED display.
*/
void setLEDPWM(i2c_inst_t *i2c, uint8_t addr, uint8_t lednum, uint8_t pwm, uint8_t bank) {
    if (lednum >= 144)
        return;
    writeRegister8(i2c, addr, bank, 0x24 + lednum, pwm);
}

/*
    This method sets the PWM for all LEDs on the LED display.
*/
void setAllLEDPWM(i2c_inst_t *i2c, uint8_t addr, uint8_t pwm, uint8_t frame) {
    selectBank(i2c, addr, frame);
    for (uint8_t lednum = 0; lednum < 144; lednum++) {
        setLEDPWM(i2c, addr, lednum, pwm, frame);
    }
}

/*
    This method sets the PWM for a specific pixel on the LED display.
*/
void setPixel(i2c_inst_t *i2c, uint8_t addr, uint8_t x, uint8_t y, uint8_t pwm, uint8_t frame) {
    if (x >= 16 || y >= 9)
        return;
    uint8_t lednum = y * 16 + x;
    setLEDPWM(i2c, addr, lednum, pwm, frame);
}