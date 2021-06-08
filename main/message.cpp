#include <string.h>
#include <stdint.h>

#include "mesh/types.hpp"
#include "mesh/node_types.hpp"
#include "mesh/node.hpp"

#include "esp_log.h"
#include "defines.hpp"

//#include "init_io.hpp"

#include "gpio.h"

#include "datetime.h"
#include "ds3231.hpp"

#include "dallas_temperature.h"

extern Dallas_Temperature temp_sensor;

extern GPIO_Basic led;
extern GPIO_Basic out1;

extern DS3231 rtc;

/**
 * This dummy function is used just as a unknown symbol to force the compilation
 * of this file. Without it the compiler just ignores this object
 *
 * more: https://esp32.com/viewtopic.php?t=9270
 */
void dummy(){}

enum class agro_commands{
	get_time = 0,
	set_time,
	get_sensor,
	get_output,
	set_output,
	uptime,
};

namespace Mesh{

/**
 * \brief Generate message that will be echoed
 *
 * Function fill buffer with header parameters and copy data
 * payload
 *
 * @param [in] h Header of received message
 * @param [out] buffer Buffer to fill reply message
 * @param [in] payload Payload of received message
 *
 * @return generated message size
 */
static size_t process_message(Node_Header* h, uint8_t* payload, uint8_t* buffer)
{
	Node_Header* header = reinterpret_cast<Node_Header*>(buffer);

	memcpy(buffer + sizeof(Node_Header*), payload, h->size);
	header->type = Message_Type::DATA;
	header->version = node_version;
	header->size = h->size;

	switch(static_cast<agro_commands>(payload[0] - '0'))
	{
		case agro_commands::get_time:
			ESP_LOGI(TAG, "Command 0");
			{
				DateTime dt;
				rtc.getDateTime(&dt);
				printf("Time: %u/%u/%u %u:%u:%u %u\n",
						dt.getDay(), dt.getMonth(), dt.getYear(),
						dt.getHour(), dt.getMinute(), dt.getSecond(),
						dt.dayOfWeek());
			}
			break;
		case agro_commands::set_time:
			ESP_LOGI(TAG, "Command 1");
			led.toggle();
			break;
		case agro_commands::get_output:
			ESP_LOGI(TAG, "Command 2");
			break;
		case agro_commands::set_output:
			ESP_LOGI(TAG, "Command 3");
			out1.toggle();
			break;
		case agro_commands::get_sensor:
			ESP_LOGI(TAG, "Command 4");
			printf("Temp(C): %f\n", temp_sensor.getTempCByIndex(0));
			break;
		case agro_commands::uptime:
			ESP_LOGI(TAG, "Command 5");
			printf("Uptime: %lld\n", esp_timer_get_time());
			break;
		default:
			ESP_LOGI(TAG, "Command Not found");
			break;
	}

	return h->size + sizeof(Node_Header);
}

/**
 * \brief Node data message handler
 *
 * This function is called every time the mesh node receives a
 * message of type data.
 *
 * \note This function will overwrite the default handler (that just prints
 * the message).
 *
 * @param [in] h Header of received message
 * @param [out] buffer Buffer to fill reply message
 * @param [in] payload Payload of received message
 * @param [in] arg Argument that (could be) set before call
 */
void node_data_handler(Node_Header* header, uint8_t* payload, uint8_t* buffer, void* arg)
{
	mesh_data_t s_data;

	s_data.data = buffer;
	s_data.size = process_message(header, payload, buffer);
	s_data.proto = MESH_PROTO_BIN;
	s_data.tos = MESH_TOS_P2P;

	esp_err_t err = Node::to_send(&s_data);
	if(err != ESP_OK)
	{
		ESP_LOGE(TAG, "[%X/%s] Error sending DATA to ROOT [%u]", err, esp_err_to_name(err), s_data.size);
	}
}

}//Mesh
