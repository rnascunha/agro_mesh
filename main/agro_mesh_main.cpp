#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "init.hpp"

extern "C" void app_main(void)
{
	nvs_flash_init();

	init_io();
	init_mesh();
	init_coap_resources();
	/**
	 * Hang there
	 */
	while(true) vTaskDelay(portMAX_DELAY);
}

