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

GPIO_Basic led(ONBOARD_LED), out1(AC_LOAD1);
GPIO_Basic water_level[]{
		GPIO_Basic{WATER_LEVEL1},
		GPIO_Basic{WATER_LEVEL2},
		GPIO_Basic{WATER_LEVEL3},
		GPIO_Basic{WATER_LEVEL4}
};

//const std::size_t water_level_count = sizeof(water_level) / sizeof(water_level[0]);

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

	led.mode(GPIO_MODE_OUTPUT);
	out1.mode(GPIO_MODE_OUTPUT);

	led.write(0);
	out1.write(0);

	i2c.init();

	rtc_present = i2c.probe(DS3231::reg);
	if(rtc_present) rtc.begin();

	ESP_LOGI(TAG, "RTC present: %s", rtc_present ? "true" : "false");
}
