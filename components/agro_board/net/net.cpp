#include "net.hpp"

#include "esp_event.h"
#include "esp_mesh.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "send.hpp"
#include "task.hpp"

#include "init_resources.hpp"

#include "esp_err.h"

#include "../types.hpp"

extern QueueHandle_t send_data_queue;
static bool init_packets_sended = false;

static TaskHandle_t send_task = NULL;
static TaskHandle_t async_task = NULL;

static void network_start(void *arg, esp_event_base_t event_base,
				int32_t event_id, void *event_data) noexcept
{
	switch (event_id)
	{
		case MESH_EVENT_PARENT_CONNECTED: {
			if(!send_task)
				xTaskCreate(coap_send_main, "send", 3072, &send_task, 5, &send_task);
			if(!async_task)
				xTaskCreate(send_async_data, "async", 2048, &async_task, 5, &async_task);
		}
		break;
		case MESH_EVENT_PARENT_DISCONNECTED: {
				Packet_Type t = Packet_Type::no_packet;
				xQueueSend(send_data_queue, &t, 0);
		}
		break;
		case MESH_EVENT_TODS_STATE: {
			mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
			bool ds_state_ = *toDs_state ? true : false;
			if(!init_packets_sended && ds_state_)
			{
				CoAP::Error ec;
		        send_board_config(CoAP::Message::type::nonconfirmable, ec);
		        init_packets_sended = true;
			}
		}
		break;
		default:
			break;
	}
}

void init_net() noexcept
{
	init_resources();
	esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &network_start, NULL);
}
