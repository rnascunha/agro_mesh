#include "gpio.h"

#include "i2c_master.h"
#include "datetime.h"
#include "ds3231.hpp"

#include "onewire.h"
#include "dallas_temperature.h"

#include "pinout.hpp"

GPIO_Basic gpio_sensor(DS18B20_DATA);
OneWire onewire(&gpio_sensor);
Dallas_Temperature temp_sensor(&onewire);

GPIO_Basic led(ONBOARD_LED), out1(TRIAC_OUTPUT1);

I2C_Master i2c(I2C_NUM_0, I2C_SCL, I2C_SDA, I2C_FAST_SPEED_HZ);
DS3231 rtc(&i2c);

void init_io() noexcept
{
	led.mode(GPIO_MODE_OUTPUT);
	out1.mode(GPIO_MODE_OUTPUT);

	led.write(0);
	out1.write(0);

	i2c.init();

	rtc.begin();

	//Hard code time
	DateTime dt;
	dt.setUnixTime(1623109458);
	rtc.setDateTime(&dt);
}
