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

//	GPIO_Basic gpio_sensor(DS18B20_DATA);
//	OneWire onewire(&gpio_sensor);
//	Dallas_Temperature temp_sensor(&onewire);
//
//	GPIO_Basic led(ONBOARD_LED), out1(TRIAC_OUTPUT1);
//	led.mode(GPIO_MODE_OUTPUT);
//	out1.mode(GPIO_MODE_OUTPUT);
//
//	GPIO_Basic wl4(WATER_LEVEL4);
//	wl4.mode(GPIO_MODE_INPUT);
//
//	printf("Water level: %d\n", wl4.read());
//
//	led.write(0);
//	out1.write(0);
//
//	I2C_Master i2c(I2C_NUM_0, I2C_SCL, I2C_SDA, I2C_FAST_SPEED_HZ);
//	i2c.init();
//
//	DS3231 rtc(&i2c);
//	rtc.begin();
//
//	DateTime dt;
//	//Hard code time
//	dt.setUnixTime(1623109458);
//	rtc.setDateTime(&dt);

	esp_err_t r;
	adc2_config_channel_atten(WATER_LEVEL_ADC4, ADC_ATTEN_11db);

	gpio_num_t adc_gpio_num;

    r = adc2_pad_get_io_num( WATER_LEVEL_ADC4, &adc_gpio_num );
	assert( r == ESP_OK );

	printf("ADC2 channel %d @ GPIO %d\n", WATER_LEVEL_ADC4, adc_gpio_num);

	vTaskDelay(2 * portTICK_PERIOD_MS);
	int     read_raw;

	while(true){
//		temp_sensor.requestTemperatures();
//		led.toggle();
//		out1.toggle();
//		rtc.getDateTime(&dt);

		r = adc2_get_raw( WATER_LEVEL_ADC4, width, &read_raw);
		if ( r == ESP_OK ) {
			printf("%d\n", read_raw );
		}
		else {
			printf("%s\n", esp_err_to_name(r));
		}

//		printf("Time: %u/%u/%u %u:%u:%u %u | Temp(C): %f | WL4: %d\n",
//				dt.getDay(), dt.getMonth(), dt.getYear(),
//				dt.getHour(), dt.getMinute(), dt.getSecond(),
//				dt.dayOfWeek(),
//				temp_sensor.getTempCByIndex(0),
//				wl4.read()
//				);

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
