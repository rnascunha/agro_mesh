#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char* TAG = "FreeRTOS INFO";

void print_freertos_tasks_name(){
	TaskStatus_t buffer[20];
	uint32_t time, num_tasks;


	num_tasks = uxTaskGetSystemState(buffer, 20, &time);
	ESP_LOGI(TAG, "FreeRTOS Tasks Names[%u]:", num_tasks);

	for(uint8_t i = 0; i < num_tasks; i++){
		ESP_LOGI(TAG, "%u: %s", i, buffer[i].pcTaskName);
	}
}
