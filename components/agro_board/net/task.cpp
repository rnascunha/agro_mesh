#include "../types.hpp"
#include "coap-te.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "agro/helper.hpp"

#include "send.hpp"

#define TAG 	"TASK"

extern QueueHandle_t send_data_queue;

/**
 * Send periodic data
 */
void coap_send_main(void* pv) noexcept
{
	while(Agro::is_networking())
	{
		CoAP::Error ec;
		send_sensors_data(CoAP::Message::type::nonconfirmable, ec);
		if(ec)
		{
			ESP_LOGE(TAG, "ERROR sending time request [%d/%s]...", ec.value(), ec.message());
		}
		else
		{
			ESP_LOGI(TAG, "Sending time request...");
		}
		vTaskDelay((5 * 60 * 1000) / portTICK_PERIOD_MS);
	}
	TaskHandle_t* handle = static_cast<TaskHandle_t*>(pv);
	*handle = NULL;
	vTaskDelete(NULL);
}

/**
 * Async send data
 */
void send_async_data(void* pv) noexcept
{
	Packet_Type type;
	while(Agro::is_networking())
	{
		if(xQueueReceive(send_data_queue, &type, portMAX_DELAY))
		{
			switch(type)
			{
				case Packet_Type::sensor:
				{
					CoAP::Error ec;
					send_sensors_data(CoAP::Message::type::nonconfirmable, ec);
				}
				break;
				default:
					break;
			}
		}
	}
	TaskHandle_t* handle = static_cast<TaskHandle_t*>(pv);
	*handle = NULL;
	vTaskDelete(NULL);
}
