#include "Motor.h"

/*
    This method initializes the PWM for pins 12 and 13 of the pico.
*/
void init_motor()
{
    //Set the GPIO functions of pins 13 and 12 to PWM.
    gpio_set_function(29, GPIO_FUNC_PWM);

    //Get the PWM slice for pin 13. This will be the same as
    //pin 12 so we just get the slice once. Set it to true.
    uint slice_num = pwm_gpio_to_slice_num(29);
    pwm_set_enabled(slice_num, true);


    //Ensure that the PWM is set to off.
    pwm_set_gpio_level(29, 0x0);
}

/*
    This method takes in a speed in percent and sets the PWM of the GPIO pin accordingly. This method expects
    the speed to be between 0 and 100. If it isn't within those bound this method will clamp the pwm.
*/
void set_motor_pwm(uint8_t speed)
{
    //Clamp the speed to be between 0 and 100.
    if(speed > 100) speed = 100;
    if(speed < 0) speed = 0;

    //Convert speed in percentage to 16-bit value that the motor register uses.
    uint16_t reg_speed = (speed * 65535) / 100;

    pwm_set_gpio_level(29, reg_speed);
}

/*

    This methods stops the motor.
*/
void stop_motor()
{
    //Set the PWM levels to zero to stop the motor.
    pwm_set_gpio_level(29, 0);
}