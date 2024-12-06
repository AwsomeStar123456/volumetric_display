#include "Initialization.h"

/*
    This method initializes the LED display pins.
*/
void init_led()
{
    gpio_set_function(0, GPIO_FUNC_SIO);
    gpio_set_function(1, GPIO_FUNC_SIO);
    gpio_set_function(2, GPIO_FUNC_SIO);
    gpio_set_function(3, GPIO_FUNC_SIO);
    gpio_set_function(4, GPIO_FUNC_SIO);
    gpio_set_function(5, GPIO_FUNC_SIO);
    gpio_set_function(6, GPIO_FUNC_SIO);
    gpio_set_function(7, GPIO_FUNC_SIO);
    gpio_set_function(8, GPIO_FUNC_SIO);
    gpio_set_function(9, GPIO_FUNC_SIO);
    gpio_set_function(10, GPIO_FUNC_SIO);
    gpio_set_function(11, GPIO_FUNC_SIO);
    gpio_set_function(12, GPIO_FUNC_SIO);
    gpio_set_function(13, GPIO_FUNC_SIO);
    gpio_set_function(14, GPIO_FUNC_SIO);
    gpio_set_function(15, GPIO_FUNC_SIO);
    gpio_set_function(26, GPIO_FUNC_SIO);
    gpio_set_function(27, GPIO_FUNC_SIO);

    gpio_disable_pulls(0);
    gpio_disable_pulls(1);
    gpio_disable_pulls(2);
    gpio_disable_pulls(3);
    gpio_disable_pulls(4);
    gpio_disable_pulls(5);
    gpio_disable_pulls(6);
    gpio_disable_pulls(7);
    gpio_disable_pulls(8);
    gpio_disable_pulls(9);
    gpio_disable_pulls(10);
    gpio_disable_pulls(11);
    gpio_disable_pulls(12);
    gpio_disable_pulls(13);
    gpio_disable_pulls(14);
    gpio_disable_pulls(15);
    gpio_disable_pulls(26);
    gpio_disable_pulls(27);


    gpio_set_dir(0, GPIO_IN);
    gpio_set_dir(1, GPIO_IN);
    gpio_set_dir(2, GPIO_IN);
    gpio_set_dir(3, GPIO_IN);
    gpio_set_dir(4, GPIO_IN);
    gpio_set_dir(5, GPIO_IN);
    gpio_set_dir(6, GPIO_IN);
    gpio_set_dir(7, GPIO_IN);
    gpio_set_dir(8, GPIO_IN);
    gpio_set_dir(9, GPIO_IN);
    gpio_set_dir(10, GPIO_IN);
    gpio_set_dir(11, GPIO_IN);
    gpio_set_dir(12, GPIO_IN);
    gpio_set_dir(13, GPIO_IN);
    gpio_set_dir(14, GPIO_IN);
    gpio_set_dir(15, GPIO_IN);
    gpio_set_dir(26, GPIO_IN);
    gpio_set_dir(27, GPIO_IN);
    
}

/*
    This method initiates the IR sensor.
*/
void init_ir_sense()
{
    //Initiate GPIO 26 to be an input pin so that we can read the value from the IR sensor.
    gpio_init(28);
    gpio_set_dir(28, GPIO_IN);
}