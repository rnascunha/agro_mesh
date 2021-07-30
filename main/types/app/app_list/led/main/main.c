
#include "driver/gpio.h"

#define ONBOARD_LED 	GPIO_NUM_2

uint32_t local_main(uint32_t arg)
{
	gpio_set_level(ONBOARD_LED, arg ? 1 : 0);
    return 0;
}
