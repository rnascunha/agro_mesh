#include <cstdint>
#include "esp_log.h"

#include "gpio.h"

#include "i2c_master.h"

#include "onewire.h"
#include "dallas_temperature.h"

#include "pinout.hpp"

#include "net/types.hpp"
#include "types/rtc_time.hpp"

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

extern const std::size_t ac_load_count = sizeof(ac_load) / sizeof(ac_load[0]);

GPIO_Basic water_level[]{
		GPIO_Basic{WATER_LEVEL1},
		GPIO_Basic{WATER_LEVEL2},
		GPIO_Basic{WATER_LEVEL3},
		GPIO_Basic{WATER_LEVEL4}
};

const std::size_t water_level_count = sizeof(water_level) / sizeof(water_level[0]);

GPIO_Basic gpio_generic[]{
		GPIO_Basic{GPIO_GENERIC1},
		GPIO_Basic{GPIO_GENERIC1},
		GPIO_Basic{GPIO_GENERIC1},
		GPIO_Basic{GPIO_GENERIC1}
};

const std::size_t gpio_generic_count = sizeof(gpio_generic) / sizeof(gpio_generic[0]);

I2C_Master i2c(I2C_NUM_0, I2C_SCL, I2C_SDA, I2C_FAST_SPEED_HZ);
std::uint8_t temp_sensor_count = false;

xQueueHandle send_data_queue = NULL;

static void IRAM_ATTR gpio_input_interrput(void*)
{
	Packet_Type t = Packet_Type::sensor;
	xQueueSendFromISR(send_data_queue, &t, NULL);
}

RTC_Time device_clock(i2c);

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
		water_level[i].pull(GPIO_PULLUP_ONLY);
		water_level[i].mode(GPIO_MODE_INPUT);
		water_level[i].enable_interrupt(GPIO_INTR_ANYEDGE);
		water_level[i].register_interrupt(gpio_input_interrput, nullptr);
	}

	for(std::size_t i = 0; i < gpio_generic_count; i++)
	{
		gpio_generic[i].pull(GPIO_PULLUP_ONLY);
		gpio_generic[i].mode(GPIO_MODE_INPUT);
		gpio_generic[i].enable_interrupt(GPIO_INTR_ANYEDGE);
		gpio_generic[i].register_interrupt(gpio_input_interrput, nullptr);
	}

	i2c.init();
	device_clock.init(i2c);

	send_data_queue = xQueueCreate(10, sizeof(Packet_Type));

	ESP_LOGI(TAG, "RTC present: %s", device_clock.has_rtc() ? "true" : "false");
}
