#include "esp_log.h"
#include "esp_mesh.h"

#include "mesh/node.hpp"
#include "mesh/border_router.hpp"
#include "mesh/types.hpp"
#include "mesh/br_types.hpp"

#include "mesh/node_message.hpp"

namespace Mesh{

void data_message(BR_Header* header, uint8_t* payload)
{
	if(header->size < sizeof(mesh_addr_t))
	{
		ESP_LOGW(TAG, "Data message too small (%u)", header->size);
		return;
	}

	BR_Message_Data* data = reinterpret_cast<BR_Message_Data*>(payload);
	uint8_t* data_payload = reinterpret_cast<uint8_t*>(payload + sizeof(BR_Message_Data));

	mesh_data_t s_data;
	s_data.size = header->size - sizeof(mesh_addr_t);
	s_data.data = data_payload;
	s_data.proto = MESH_PROTO_BIN;
	s_data.tos = MESH_TOS_P2P;

	esp_err_t err = esp_mesh_send((mesh_addr_t*)&data->addr, &s_data, MESH_DATA_FROMDS, NULL, 0);
	if(err != ESP_OK)
	{
		ESP_LOGE(TAG, "[%d] Error sending DATA to node " MACSTR " [%s]",
				(err), MAC2STR(data->addr.addr), esp_err_to_name(err));
	} else {
		ESP_LOGI(TAG, "Data Router to[%u]: " MACSTR, s_data.size, MAC2STR(data->addr.addr));
	}
}

static void waive_root()
{
	esp_mesh_waive_root(NULL, MESH_VOTE_REASON_ROOT_INITIATED);
}

void command_message(BR_Header* header, uint8_t* payload){
	if(header->size < sizeof(BR_Command_Type))
	{
		ESP_LOGW(TAG, "Data message too small (%u)", header->size);
		return;
	}

	BR_Message_Command* comm = reinterpret_cast<BR_Message_Command*>(payload);
	switch(comm->command)
	{
		case BR_Command_Type::ROUTE_TABLE:
			ESP_LOGI(TAG, "[COMMAND] Router table");
			send_route_table();
			break;
		case BR_Command_Type::CONFIG:
			ESP_LOGI(TAG, "[COMMAND] Config");
			send_config();
			break;
		case BR_Command_Type::STATUS:
			ESP_LOGI(TAG, "[COMMAND] Status");
			send_status();
			break;
		case BR_Command_Type::FULL_CONFIG:
			ESP_LOGI(TAG, "[COMMAND] Full config");
			send_full_config();
			break;
		case BR_Command_Type::REBOOT:
			ESP_LOGI(TAG, "[COMMAND] Reboot");
			esp_restart();
			break;
		case BR_Command_Type::WAIVE_ROOT:
			ESP_LOGI(TAG, "[COMMAND_BR] Waive root");
			waive_root();
			break;
		default:
//			ESP_LOGI(TAG, "Command not defined [%X]", comm->command);
			break;
	}
}

}//Mesh
