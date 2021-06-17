#include <stdio.h>

//#include "print_system_info.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "gpio.h"

#include "i2c_master.h"
#include "datetime.h"
#include "ds3231.hpp"

#include "onewire.h"
#include "dallas_temperature.h"

#include "pinout.hpp"

#include "driver/adc.h"

static const char* TAG = "APP MAIN";

static const adc_bits_width_t width = ADC_WIDTH_BIT_12;

extern "C" void app_main(void)
{
	ESP_LOGI(TAG, "Iniciando!");

	GPIO_Basic gpio_sensor(DS18B20_DATA);
	OneWire onewire(&gpio_sensor);
	Dallas_Temperature temp_sensor(&onewire);

	GPIO_Basic led(ONBOARD_LED, GPIO_MODE_OUTPUT),
			out1(AC_LOAD1, GPIO_MODE_OUTPUT);
	GPIO_Basic wl4(WATER_LEVEL4, GPIO_MODE_INPUT);

	led.write(0);
	out1.write(0);

	I2C_Master i2c(I2C_NUM_0, I2C_SCL, I2C_SDA, I2C_FAST_SPEED_HZ);
	i2c.init();

	DS3231 rtc(&i2c);
	rtc.begin();

	DateTime dt;
	//Hard code time
	dt.setUnixTime(1623109458);
	rtc.setDateTime(&dt);

	while(true){
		temp_sensor.requestTemperatures();
		led.toggle();
		out1.toggle();
		rtc.getDateTime(&dt);

		printf("Time: %u/%u/%u %u:%u:%u %u | Temp(C): %f | WL4: %d\n",
				dt.getDay(), dt.getMonth(), dt.getYear(),
				dt.getHour(), dt.getMinute(), dt.getSecond(),
				dt.dayOfWeek(),
				temp_sensor.getTempCByIndex(0),
				wl4.read()
				);

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
