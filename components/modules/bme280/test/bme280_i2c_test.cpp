#include "driver/gpio.h"
#include "i2c_master.h"
#include "bme280_i2c.h"

#include "hardware_defs.h"

#include <stdio.h>

#include "esp_log.h"

static const char* TAG = "APP_MAIN";

#define	SCL_PIN			GPIO_NUM_25
#define	SDA_PIN			GPIO_NUM_26

#define I2C_FREQ_HZ		100000

//#define BME280_TEST_FORCED_MODE

void print_sensor_data(const struct BME280::data *comp_data){
#ifdef BME280_FLOAT_ENABLE
	printf("%f, %f, %f\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
#else /* BME280_FLOAT_ENABLE */
	printf("Temp: %d/%d.%d |Pressure: %u/%u |humidity: %u/%u\n",comp_data->temperature, comp_data->temperature / 100, comp_data->temperature % 100,
			comp_data->pressure, comp_data->pressure / 100,
			comp_data->humidity, comp_data->humidity / 1024);
#endif /* BME280_FLOAT_ENABLE */
}

extern "C" void app_main(){
	I2C_Master i2c(I2C_NUM_0, SCL_PIN, SDA_PIN, I2C_FREQ_HZ);
	i2c.init();

	BME280_I2C	sensor(&i2c);

	int ret = sensor.init();

	if(ret < 0){
		ESP_LOGI(TAG, "Error initalize sensor %d", ret);
		return;
	} else {
		ESP_LOGI(TAG, "Initalized sensor addr 0x%02X", ret);
	}

	BME280::settings	settings;
	BME280::data		data;

    /* Recommended mode of operation: Indoor navigation */
	settings.desired_set = BME280_OSR_PRESS_SEL
			| BME280_OSR_TEMP_SEL
			| BME280_OSR_HUM_SEL
			| BME280_FILTER_SEL
#ifndef BME280_TEST_FORCED_MODE
			| BME280_STANDBY_SEL
#endif
			;
    settings.osr_h = BME280::OVERSAMPLING_X1;
    settings.osr_p = BME280::OVERSAMPLING_X16;
    settings.osr_t = BME280::OVERSAMPLING_X2;
    settings.filter = BME280::FILTER_COEF_OFF;
#ifndef BME280_TEST_FORCED_MODE
	settings.standby_time = BME280::STANDBY_TIME_1000_MS;
#endif

    ret = sensor.config(&settings);
    if(ret < 0){
    	ESP_LOGI(TAG, "Erro configuring sensor %d", ret);
    	return;
    }

    ESP_LOGI(TAG, "Temperature, Pressure, Humidity\r\n");

#ifndef BME280_TEST_FORCED_MODE
	ret = sensor.set_mode(BME280::NORMAL);
	if(ret < 0){
		ESP_LOGI(TAG, "Erro setting sensor mode %d", ret);
		return;
	}
#endif

    /* Continuously stream sensor data */
    while (true) {
#ifdef BME280_TEST_FORCED_MODE
    	ret = sensor.set_mode(BME280::FORCED);
        if(ret < 0){
        	ESP_LOGI(TAG, "Erro setting sensor mode %d", ret);
        	return;
        }
#endif
        /* Wait for the measurement to complete and print data @25Hz */
        delay_ms(1000);
        ret = sensor.get_data(BME280_ALL, &data);
        if(ret < 0){
        	ESP_LOGI(TAG, "Erro getting sensor data %d", ret);
        	return;
        }

        print_sensor_data(&data);
    }

}

