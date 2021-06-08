#include "restart_async.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stddef.h>

static void restart_async_task(void* pvParameter){
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	esp_restart();
}

void restart_async(){
    xTaskCreate(&restart_async_task, "restart_task",
    		1024,
			NULL, 5, NULL);
}
