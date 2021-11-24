#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "xgzp6897d.hpp"

#include "agro/helper.hpp"

#define TAG	"PRESSURE"

extern I2C_Master i2c;

#define GRAVITY		9.8
#define DELTA_H		0.1
#define WATER_Kp	0.000214
#define NORM_TEMP_C	20

static double calculate_density(double diff_pressure) noexcept
{
	return diff_pressure / (GRAVITY * DELTA_H);
}

static double calculate_density_normalize(double density, double temperature) noexcept
{
	return density * (1 + (WATER_Kp * (temperature - NORM_TEMP_C)));
}

void sensor_pressure_task(void*) noexcept
{
	/**
	 * Pressure
	 */
	XGZP6897D pressure(i2c);
	if(!i2c.probe(XGZP6897D::address))
	{
		ESP_LOGW(TAG, "Pressure sensor not present.");
		vTaskDelete(NULL);
		return;
	}

	ESP_LOGI(TAG, "Initiating sensor...");
	int ret = pressure.init();
	if(ret < 0)
	{
		ESP_LOGE(TAG, "Error initiating sensor");
		vTaskDelete(NULL);
		return;
	}

	ESP_LOGI(TAG, "Setting sampling...");
	ret = pressure.set_sampling(XGZP6897D::sampling::x32768);
	if(ret < 0)
	{
		ESP_LOGW(TAG, "Error setting sampling...");
		vTaskDelete(NULL);
		return;
	}

	while(true)
	{
		vTaskDelay((60 * 1000) / portTICK_RATE_MS);
		if(!Agro::is_networking())
		{
			continue;
		}

		int ret = pressure.start_conversion(XGZP6897D::measurement_control::temp_pressure_x1);
		if(ret < 0)
		{
			ESP_LOGE(TAG, "Error initiating conversion");
			continue;
		}

		while(!pressure.check_conversion())
		{
			printf(".");
		}
		printf("\n");

		uint16_t temp;
		uint32_t press;
		ret = pressure.read_adc(temp, press);
		if(ret < 0)
		{
			ESP_LOGE(TAG, "Error reading data");
			continue;
		}

		uint8_t data[32];
		Sensor_Builder builder{data, 32};

		double temperature = XGZP6897D::convert_temperature(temp),
				pressure = XGZP6897D::convert_pressure(press, 2048),
				density = calculate_density(pressure),
				density_norm = calculate_density_normalize(density, temperature);

		builder.add(SENSOR_TYPE_TEMP, 1, static_cast<float>(temperature));
		builder.add(SENSOR_TYPE_PRESSURE, static_cast<float>(pressure));
		builder.add(7, 0, static_cast<float>(density));
		builder.add(7, 1, static_cast<float>(density_norm));

		CoAP::Error ec;
		send_sensors_data(CoAP::Message::type::nonconfirmable, builder, ec);
	}
	vTaskDelete(NULL);
}

void init_sensor_pressure_task() noexcept
{
	xTaskCreate(sensor_pressure_task, "pressure", 3072, NULL, 5, NULL);
}
