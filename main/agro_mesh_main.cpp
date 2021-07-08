#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "init.hpp"
//#include "gpio.h"

//extern GPIO_Basic led;
//void heartbeat(void*)
//{
//	while(true)
//	{
//		led.toggle();
//		vTaskDelay(2000 / portTICK_RATE_MS);
//	}
//}

extern "C" void app_main(void)
{
	nvs_flash_init();

	init_io();
	init_mesh();
	init_coap_resources();

//	xTaskCreate(&heartbeat, "heart", 512, NULL, 5, NULL);
	/**
	 * Hang there
	 */
	while(true) vTaskDelay(portMAX_DELAY);
}

