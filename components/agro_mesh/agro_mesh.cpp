#include "nvs_flash.h"

#include "agro/mesh.hpp"

#include "init.hpp"
#include "storage.hpp"

#if CONFIG_ENABLE_DEVICE_CLOCK
#include "modules/clock/rtc_time.hpp"

Agro::RTC_Time device_clock;
#endif /* CONFIG_ENABLE_DEVICE_CLOCK */

#if CONFIG_ENABLE_JOBS_SUPPORT
#include "modules/job/job_task.hpp"
#endif /* CONFIG_ENABLE_JOBS_SUPPORT */

#ifdef CONFIG_ENABLE_HEARTBEAT_TASK
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "blink.hpp"

#define BLINK_LED			((gpio_num_t)CONFIG_HEARTBEAT_GPIO)

volatile int interval_blink = BLINK_MS_NOT_CONNECTED;
static void heartbeat(void*)
{
	gpio_set_direction(BLINK_LED, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_level(BLINK_LED, 1);

	while(true)
	{
		gpio_set_level(BLINK_LED, !gpio_get_level(BLINK_LED));
		vTaskDelay(interval_blink / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
#endif /* CONFIG_ENABLE_HEARTBEAT_TASK */

namespace Agro{

void init() noexcept
{
	mount_storage();

#if CONFIG_ENABLE_DEVICE_CLOCK
	device_clock.init();
#endif /* CONFIG_ENABLE_DEVICE_CLOCK */
	init_mesh();
	init_coap_resources();

#if CONFIG_ENABLE_JOBS_SUPPORT
	Agro::Jobs::init_task();
#endif /* CONFIG_ENABLE_JOBS_SUPPORT */

#ifdef CONFIG_ENABLE_HEARTBEAT_TASK
	xTaskCreate(&heartbeat, "heart", 768, NULL, 5, NULL);
#endif /* CONFIG_ENABLE_HEARTBEAT_TASK */
}

}//Agro
