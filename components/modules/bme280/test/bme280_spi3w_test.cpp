#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "bme280_spi3w.h"

#include "hardware_defs.h"

#include <stdio.h>

#include "esp_log.h"

static const char* TAG = "APP_MAIN";

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

#define SCK					27
#define SDI					13
//#define SDO					12		//SDO floating
#define CSB					14

#define SCLK				SCK
#define CS					CSB
//#define MISO				SDO
#define MOSI				SDI

#define SPI_HOST_MODULE			HSPI_HOST
#define SPI_CLOCK				SPI_MASTER_FREQ_10M

extern "C" void app_main(){
//	spi_device_handle_t device;
//
//	ME280_SPI_3W::spi_init(SPI_HOST_MODULE,
//			SCLK, MOSI, CS,
//			SPI_CLOCK, &device);
//	BME280_SPI_3W sensor(device);

	BME280_SPI_3W sensor(SPI_HOST_MODULE, SCLK, MOSI, CS);

	int ret = sensor.init();
	if(ret < 0){
		ESP_LOGI(TAG, "Error initalize sensor %d", ret);
		return;
	} else {
		ESP_LOGI(TAG, "Initalized sensor");
	}

	uint8_t id = sensor.get_chip_id();
	if(id != BME280::CHIP_ID){
		ESP_LOGI(TAG, "error getting chip id 0x%02X", id);
		return;
	} else {
		ESP_LOGI(TAG, "chip id: 0x%02X", id);
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

