
#include "driver/gpio.h"
#include "../../../../../pinout.hpp"

#define NOP					0
#define OFF					1
#define ON					2

#define SHIFT(x, arg)		((arg >> (2 * (x - 1))) & 0b11)

static inline void set_output(gpio_num_t num, uint32_t arg)
{
	if(arg == OFF)
		gpio_set_level(num, 0);
	else if(arg == ON)
		gpio_set_level(num, 1);
}

uint32_t local_main(uint32_t arg)
{
	set_output(AC_LOAD1, SHIFT(1, arg));
	set_output(AC_LOAD2, SHIFT(2, arg));
	set_output(AC_LOAD3, SHIFT(3, arg));

    return 0;
}
