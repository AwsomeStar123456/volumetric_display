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
#include "IS31FL3731.h"

#define MTR_LEFT 0
#define MTR_RIGHT 1

#define RPM_AVERAGE_COUNT 5

volatile uint32_t last_times[RPM_AVERAGE_COUNT] = {0};
volatile uint32_t rpm = 0;
volatile uint32_t rpm_index = 0;


void init_i2c();
void init_motor();
void set_motor_pwm(bool direction, uint8_t speed);
void stop_motor();
void init_ir_sense();
void person();
void display_numbers(int numbers[4]);
void display_number(int number, int position, int frame);

void calc_rpm(uint gpio, uint32_t events);

volatile uint8_t frame;
volatile bool core1_uninitialized = true;
volatile bool rpm_updated = false;

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

void init_motor()
{
    // PWM
    gpio_set_function(13, GPIO_FUNC_PWM);
    gpio_set_function(12, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(13); // Get the PWM slice for the GPIO pin
    pwm_set_enabled(slice_num, true);

    pwm_set_gpio_level(13, 0x0);
    pwm_set_gpio_level(12, 0x0);

    // High Pin for Sleep
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_put(11, 1);
}

bool timer_callback(repeating_timer_t *rt) {
    if (!rpm_updated) {
        // No falling edge detected since last update, set RPM to zero
        rpm = 0;
    }

    rpm_updated = false;

    // Must return true to keep the timer repeating
    return true;
}

void init_timer() {
    // Set up our repeating timer
    static repeating_timer_t timer;
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);
}

//Read from time and set the global RPM variable
void calc_rpm(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (last_times[rpm_index] != 0) {
        // Calculate the time for one revolution (in microseconds)
        uint32_t revolution_time = current_time - last_times[rpm_index];

        // Calculate RPM: 60 seconds per minute, 1e6 microseconds per second
        rpm = 60 * 1e6 / revolution_time;
    }

    last_times[rpm_index] = current_time;
    rpm_index = (rpm_index + 1) % RPM_AVERAGE_COUNT;
    rpm_updated = true;
}


void init_ir_sense()
{
    // IR Sensor
    gpio_init(26);
    gpio_set_dir(26, GPIO_IN);

    gpio_set_irq_enabled_with_callback(26, GPIO_IRQ_EDGE_FALL, true, &calc_rpm);
}

void set_motor_pwm(bool direction, uint8_t speed) // Speed in terms of percentage
{
    // Convert speed in percentage to 16-bit value
    uint16_t reg_speed = (speed * 65535) / 100;
    
    if (direction == MTR_LEFT)
    {
        pwm_set_gpio_level(13, reg_speed);
        pwm_set_gpio_level(12, 0);
    }
    else // Right
    {
        pwm_set_gpio_level(13, 0);
        pwm_set_gpio_level(12, reg_speed);
    }
}

void stop_motor()
{
    pwm_set_gpio_level(13, 0);
    pwm_set_gpio_level(12, 0);
}

void core1_entry() {
    //Initalize
    init_ir_sense();
    init_timer();


    core1_uninitialized = false;
    while (1) {

        set_motor_pwm(MTR_LEFT, 100);
        // sleep_ms(2000);
        // stop_motor();  
        // sleep_ms(2000);
        // set_motor_pwm(MTR_RIGHT, 100);
        // sleep_ms(2000);
        // stop_motor();
        // sleep_ms(2000);

    }
}

int main()
{
    //Initialize
    stdio_init_all();
    init_i2c();
    init_ledDisplay(0);
    init_motor();
    clear(i2c1, ISSI_ADDR_DEFAULT, frame);
    //person();

    //Launch Core 1
    multicore_launch_core1(core1_entry);
    while(core1_uninitialized) {}

    while (1) {
        // //For Person
        // displayFrame(i2c1, ISSI_ADDR_DEFAULT, 0);
        // sleep_ms(500);
        // displayFrame(i2c1, ISSI_ADDR_DEFAULT, 1);
        // sleep_ms(500);


        // if(rpm > 500) {
        //     displayFrame(i2c1, ISSI_ADDR_DEFAULT, 0);
        // } else {
        //     displayFrame(i2c1, ISSI_ADDR_DEFAULT, 1);
        // }

        clear(i2c1, ISSI_ADDR_DEFAULT, frame);

        // Convert RPM to array of digits
        int numbers[4] = {0, 0, 0, 0};  // Initialize to 0000
        int temp_rpm = rpm;
        for (int i = 3; i >= 0 && temp_rpm > 0; i--) {
            numbers[i] = temp_rpm % 10;
            temp_rpm /= 10;
        }

        display_numbers(numbers);
        sleep_ms(1000);
    }

    return 0;
}

void person(){
    frame = 0;

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 1, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 7, 100, frame);

    frame = 1;

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 1, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 7, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);

    frame = 2;

    clear(i2c1, ISSI_ADDR_DEFAULT, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 1, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 7, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 2, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 2, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 3, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 3, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 6, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 7, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 8, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 10, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 11, 4, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 12, 4, 100, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 2, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 5, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 9, 5, 255, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 13, 5, 255, frame);

    setPixel(i2c1, ISSI_ADDR_DEFAULT, 3, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 4, 6, 100, frame);
    setPixel(i2c1, ISSI_ADDR_DEFAULT, 14, 6, 100, frame);
}

/*int digits[10][7][4] = {
    {
        {0, 1, 1, 0},  // 0
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    },
    {
        {0, 0, 1, 0},  // 1
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 1, 1, 1}
    },
    {
        {0, 1, 1, 0},  // 2
        {1, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 1}
    },
    {
        {1, 1, 1, 0},  // 3
        {0, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1},
        {1, 1, 1, 0}
    },
    {
        {0, 0, 1, 1},  // 4
        {0, 1, 0, 1},
        {1, 0, 0, 1},
        {1, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1}
    },
    {
        {0, 1, 1, 0},  // 5
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    },
    {
        {0, 1, 1, 0},  // 6
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    },
    {
        {1, 1, 1, 1},  // 7
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0}
    },
    {
        {0, 1, 1, 0},  // 8
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0}
    },
    {
        {0, 1, 1, 0},  // 9
        {1, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 0}
    }
};*/

int digits[10][7][4] = {
    {
        {0, 0, 0, 0},  // 0
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 1
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    },
    {
        {0, 0, 0, 0},  // 2
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 3
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 4
        {0, 0, 0, 0},
        {0, 1, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1}
    },
    {
        {0, 0, 0, 0},  // 5
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 6
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 7
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    },
    {
        {0, 0, 0, 0},  // 8
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1}
    },
    {
        {0, 0, 0, 0},  // 9
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 1},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 1}
    }
};

void display_number(int number, int position, int frame) {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            if (digits[number][i][j]) {
                setPixel(i2c1, ISSI_ADDR_DEFAULT, position + j, i + 1, 100, frame);
            }
        }
    }
}

void display_numbers(int numbers[4]) {
    for (int i = 0; i < 4; i++) {
        display_number(numbers[i], 4 * i, frame);
    }
}