#include <stdio.h>

#include "print_system_info.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "gpio.h"
#include "onewire.h"
#include "dallas_temperature.h"

//#include "get_type_string.h"

static const char* TAG = "APP MAIN";

#define PIN_SENSOR			GPIO_NUM_16

extern "C" void app_main(void)
{
	print_system_reset_reason();

	ESP_LOGI(TAG, "Iniciando!");

	GPIO_Basic gpio_sensor(PIN_SENSOR);
	OneWire onewire(&gpio_sensor);
	Dallas_Temperature dallas(&onewire);


	while(true){
		dallas.requestTemperatures();
		printf("Temp(C): %f\n", dallas.getTempCByIndex(0));
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}
