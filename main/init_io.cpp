#include <cstdint>
#include "esp_log.h"

#include "gpio.h"

#include "i2c_master.h"
#include "datetime.h"
#include "ds3231.hpp"

#include "onewire.h"
#include "dallas_temperature.h"

#include "pinout.hpp"

#define TAG		"AGRO_IO"

GPIO_Basic gpio_sensor(DS18B20_DATA);
OneWire onewire(&gpio_sensor);
Dallas_Temperature temp_sensor(&onewire);

GPIO_Basic led(ONBOARD_LED);
GPIO_Basic ac_load[]{
		GPIO_Basic{AC_LOAD1},
		GPIO_Basic{AC_LOAD2},
		GPIO_Basic{AC_LOAD3}
};

const std::size_t ac_load_count = sizeof(ac_load) / sizeof(ac_load[0]);

GPIO_Basic water_level[]{
		GPIO_Basic{WATER_LEVEL1},
		GPIO_Basic{WATER_LEVEL2},
		GPIO_Basic{WATER_LEVEL3},
		GPIO_Basic{WATER_LEVEL4}
};

const std::size_t water_level_count = sizeof(water_level) / sizeof(water_level[0]);

I2C_Master i2c(I2C_NUM_0, I2C_SCL, I2C_SDA, I2C_FAST_SPEED_HZ);
DS3231 rtc(&i2c);

bool rtc_present = false;
std::uint8_t temp_sensor_count = false;

void init_io() noexcept
{
	ESP_LOGI(TAG, "Initializing IOs");

	temp_sensor.begin();
	temp_sensor_count = temp_sensor.getDeviceCount();

	ESP_LOGI(TAG, "Temp sensors count: %u", temp_sensor_count);

	led.mode(GPIO_MODE_INPUT_OUTPUT);
	led.write(0);

	for(std::size_t i = 0; i < ac_load_count; i++)
	{
		ac_load[i].mode(GPIO_MODE_INPUT_OUTPUT);
		ac_load[i].write(0);
	}

	for(std::size_t i = 0; i < water_level_count; i++)
	{
		water_level[i].mode(GPIO_MODE_INPUT);
	}

	i2c.init();

	rtc_present = i2c.probe(DS3231::reg);
	if(rtc_present) rtc.begin();

	ESP_LOGI(TAG, "RTC present: %s", rtc_present ? "true" : "false");
}
