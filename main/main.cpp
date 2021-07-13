#include "nvs_flash.h"
#include "init.hpp"
#include "storage.hpp"

#ifdef CONFIG_ENABLE_HEARTBEAT_TASK
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.h"

extern GPIO_Basic led;
void heartbeat(void*)
{
	while(true)
	{
		led.toggle();
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
}

#endif /* CONFIG_ENABLE_HEARTBEAT_TASK */

extern "C" void app_main(void)
{
	nvs_flash_init();

	init_io();
	mount_storage();
	init_mesh();
	init_coap_resources();

#ifdef CONFIG_ENABLE_HEARTBEAT_TASK
	xTaskCreate(&heartbeat, "heart", 512, NULL, 5, NULL);
#endif /* CONFIG_ENABLE_HEARTBEAT_TASK */
}
