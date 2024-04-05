#ifndef DisplayAddons
#define DisplayAddons

#include <stdint.h>  // for uint8_t
#include <stdbool.h> // for bool
#include "hardware/i2c.h"  // for i2c_inst_t

extern volatile uint8_t frame;

void person();
void display_number(int number, int position, int frame);
void display_numbers(int numbers[4]);

#endif