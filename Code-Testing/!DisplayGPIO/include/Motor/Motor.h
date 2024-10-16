#ifndef Initialization
#define Initialization

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

void init_motor();
void set_motor_pwm(uint8_t speed);
void stop_motor();

#endif