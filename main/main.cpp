#include "nvs_flash.h"
#include "init.hpp"
#include "storage.hpp"
#include "job.hpp"
#include "mbedtls/md.h"
#include "sensor/sensor_pressure_task.hpp"

#ifdef CONFIG_ENABLE_HEARTBEAT_TASK
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.h"

#include "blink.hpp"

extern GPIO_Basic led;
volatile int interval_blink = BLINK_MS_NOT_CONNECTED;
void heartbeat(void*)
{
	while(true)
	{
		led.toggle();
		vTaskDelay(interval_blink / portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}
#endif /* CONFIG_ENABLE_HEARTBEAT_TASK */

extern "C" void app_main(void)
{
	nvs_flash_init();

	init_io();
	mount_storage();
	init_mesh();
	init_coap_resources();
//	init_job_task();

	init_sensor_pressure_task();

#ifdef CONFIG_ENABLE_HEARTBEAT_TASK
	xTaskCreate(&heartbeat, "heart", 768, NULL, 5, NULL);
#endif /* CONFIG_ENABLE_HEARTBEAT_TASK */
}

