#ifndef IS31FL3731
#define IS31FL3731

#include <stdint.h>  // for uint8_t
#include <stdbool.h> // for bool
#include "hardware/i2c.h"  // for i2c_inst_t

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

void init_ledDisplay(uint8_t frame);
bool writeRegister8(i2c_inst_t *i2c, uint8_t addr, uint8_t bank, uint8_t reg, uint8_t data);
bool selectBank(i2c_inst_t *i2c, uint8_t addr, uint8_t bank);
void displayFrame(i2c_inst_t *i2c, uint8_t addr, uint8_t frame);
void clear(i2c_inst_t *i2c, uint8_t addr, uint8_t frame);
void setLEDPWM(i2c_inst_t *i2c, uint8_t addr, uint8_t lednum, uint8_t pwm, uint8_t bank);
void setAllLEDPWM(i2c_inst_t *i2c, uint8_t addr, uint8_t pwm, uint8_t frame);
void setPixel(i2c_inst_t *i2c, uint8_t addr, uint8_t x, uint8_t y, uint8_t pwm, uint8_t frame);

#endif