#include "read_sensor.hpp"

#include "dallas_temperature.h"
#include "gpio.h"

extern Dallas_Temperature temp_sensor;
extern GPIO_Basic water_level[];
extern GPIO_Basic gpio_generic[];
extern GPIO_Basic ac_load[];

unsigned read_gpios() noexcept
{
	unsigned bit_array = 0;
	bit_array |= (water_level[0].read() << 0);
	bit_array |= (water_level[1].read() << 1);
	bit_array |= (water_level[2].read() << 2);
	bit_array |= (water_level[3].read() << 3);

	bit_array |= (gpio_generic[0].read() << 4);
	bit_array |= (gpio_generic[1].read() << 5);
	bit_array |= (gpio_generic[2].read() << 6);
	bit_array |= (gpio_generic[3].read() << 7);

	bit_array |= (ac_load[0].read() << 8);
	bit_array |= (ac_load[1].read() << 9);
	bit_array |= (ac_load[2].read() << 10);

	return bit_array;
}

float read_temperature_retry(unsigned retry_times) noexcept
{
	do{
		temp_sensor.requestTemperatures();
		float temp = temp_sensor.getTempCByIndex(0);
		if(temp != INVALID_TEMPERATURE)
			return temp;
	}while(retry_times--);

	return INVALID_TEMPERATURE;
}
