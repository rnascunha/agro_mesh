#include "esp_log.h"
#include "esp_mesh.h"

#include "mesh/node.hpp"
#include "mesh/types.hpp"
#include "mesh/node_types.hpp"

#include "lwip/sockets.h"

namespace Mesh{

#define TAG				"MESH_NODE"

#define TX_SIZE			1024
uint8_t tx_buf[1024];

static void data_message(Node_Header* header, uint8_t* payload, void*);
static void command_message(Node_Header* header, uint8_t* payload);
static size_t make_route_table(uint8_t* buf);
static size_t make_config(uint8_t* buf);
static size_t make_status(uint8_t* buf);
static size_t make_full_config(uint8_t* buf);
static size_t make_info(uint8_t* buf, const char* info);

void __attribute__((weak))
node_fromds_message(mesh_addr_t* from, mesh_data_t* data, void* arg)
{
	if(data->size < sizeof(Node_Header)){
		ESP_LOGE(TAG, "Message too small [%u]", data->size);
		return;
	}

	Node_Header* header = reinterpret_cast<Node_Header*>(data->data);
	if(header->version != node_version){
		ESP_LOGE(TAG, "Invalid message version sent [%u]", header->version);
		return;
	}

	switch(header->type){
		case Message_Type::DATA:
			data_message(header, reinterpret_cast<uint8_t*>(data->data + sizeof(Node_Header)), arg);
			break;
		case Message_Type::COMMAND:
			ESP_LOGI(TAG, "Message command [%u]", header->size);
			command_message(header, reinterpret_cast<uint8_t*>(data->data + sizeof(Node_Header)));
			break;
		default:
			ESP_LOGI(TAG, "Message type not defined [%u]", (uint8_t)header->type);
			break;
	}
}

void __attribute__((weak))
node_tods_message(mesh_addr_t* from, mesh_data_t* data, void* arg)
{
	ESP_LOGI(TAG, "TODS message [>" MACSTR "][%u]: ", MAC2STR(from->addr), data->size);
	for(int i = 0; i < data->size; i++)
		printf("%02X ", data->data[i]);

	printf("\n");
}

void send_route_table()
{
	mesh_data_t s_data;
	s_data.data = tx_buf;
	s_data.size = make_route_table(tx_buf);
	s_data.proto = MESH_PROTO_BIN;
	s_data.tos = MESH_TOS_P2P;

	esp_err_t err = Node::to_send(&s_data);
	if(err != ESP_OK)
		ESP_LOGE(TAG, "[%X/%s] Error sending DATA to ROOT", err, esp_err_to_name(err));
}

void send_config()
{
	mesh_data_t s_data;
	s_data.data = tx_buf;
	s_data.size = make_config(tx_buf);
	s_data.proto = MESH_PROTO_BIN;
	s_data.tos = MESH_TOS_P2P;

	esp_err_t err = Node::to_send(&s_data);
	if(err != ESP_OK)
		ESP_LOGE(TAG, "[%X/%s] Error sending DATA to ROOT [%u]", err, esp_err_to_name(err), s_data.size);
}

void send_status()
{
	mesh_data_t s_data;
	s_data.data = tx_buf;
	s_data.size = make_status(tx_buf);
	s_data.proto = MESH_PROTO_BIN;
	s_data.tos = MESH_TOS_P2P;

	esp_err_t err = Node::to_send(&s_data);
	if(err != ESP_OK)
		ESP_LOGE(TAG, "[%X/%s] Error sending DATA to ROOT", err, esp_err_to_name(err));
}

void send_full_config()
{
	mesh_data_t s_data;
	s_data.data = tx_buf;
	s_data.size = make_full_config(tx_buf);
	s_data.proto = MESH_PROTO_BIN;
	s_data.tos = MESH_TOS_P2P;

	esp_err_t err = Node::to_send(&s_data);
	if(err != ESP_OK)
		ESP_LOGE(TAG, "[%d/%s] Error sending DATA to ROOT [%u]", err, esp_err_to_name(err), s_data.size);
}

void send_info(const char* info)
{
	mesh_data_t s_data;

	s_data.data = tx_buf;
	s_data.size = make_info(tx_buf, info);
	s_data.proto = MESH_PROTO_BIN;
	s_data.tos = MESH_TOS_P2P;

	esp_err_t err = Node::to_send(&s_data);
	if(err != ESP_OK)
		ESP_LOGE(TAG, "[%X/%s] Error sending DATA to ROOT [%u]", err, esp_err_to_name(err), s_data.size);
}

void __attribute__((weak))
node_data_handler(Node_Header* header, uint8_t* payload, uint8_t* buffer, void* arg)
{
	ESP_LOGI(TAG, "Message data [%u]: %.*s", header->size, header->size, (char*)payload);
}

static void data_message(Node_Header* header, uint8_t* payload, void* arg)
{
	node_data_handler(header, payload, tx_buf, arg);
}

static void make_header(Node_Header* header, Message_Type type, uint16_t size)
{
	header->version = node_version;
	header->type = type;
	header->size = size;
}

static size_t make_info(uint8_t* buf, const char* info)
{
	Response_Command* comm = reinterpret_cast<Response_Command*>(buf);

	comm->type = Command_Type::INFO;

	size_t len = strlen(info);
	memcpy(buf + sizeof(Response_Command), info, len);

	make_header(&comm->header, Message_Type::COMMAND, len);

	return sizeof(Response_Command) + len;
}

static size_t make_config(uint8_t* buf)
{
	Response_Config* msg = reinterpret_cast<Response_Config*>(buf);

	mesh_cfg_t cfg;
	wifi_ap_record_t ap;

	esp_mesh_get_config(&cfg);
	esp_wifi_sta_get_ap_info(&ap);

	memcpy(&msg->id, &cfg.mesh_id.addr, 6);
	Node::mac_ap(&msg->mac_ap);
	msg->channel_config = cfg.channel;
	msg->channel_conn = ap.primary;
	msg->is_root = Node::is_root();

	msg->command = Command_Type::CONFIG;

	make_header(&msg->header, Message_Type::COMMAND, sizeof(Response_Config) - sizeof(Node_Header));

	return sizeof(Response_Config);
}

static size_t make_route_table(uint8_t* buf)
{
	Response_Route_Table* msg = reinterpret_cast<Response_Route_Table*>(buf);

	msg->command = Command_Type::ROUTE_TABLE;
	msg->layer = static_cast<uint8_t>(Node::layer());
	Node::parent(&msg->parent);

	int route_table_size = 0;
	esp_mesh_get_routing_table((mesh_addr_t *)(tx_buf + sizeof(Response_Route_Table)),
	                                   CONFIG_MESH_ROUTE_TABLE_SIZE * 6, &route_table_size);

	make_header(&msg->header, Message_Type::COMMAND, sizeof(Response_Route_Table)
														- sizeof(Node_Header)
														+ route_table_size * 6);

	return msg->header.size + sizeof(Node_Header);
}

static size_t make_status(uint8_t* buf)
{
	Response_Status* msg = reinterpret_cast<Response_Status*>(buf);

	wifi_ap_record_t ap;
	esp_wifi_sta_get_ap_info(&ap);
	msg->rssi = ap.rssi;
	msg->command = Command_Type::STATUS;

	make_header(&msg->header, Message_Type::COMMAND, sizeof(Response_Status) - sizeof(Node_Header));

	return sizeof(Response_Status);
}

static size_t make_full_config(uint8_t* buf)
{
	Response_Full_Config* msg = reinterpret_cast<Response_Full_Config*>(buf);

	wifi_ap_record_t ap;
	mesh_cfg_t cfg;

	esp_mesh_get_config(&cfg);
	esp_wifi_sta_get_ap_info(&ap);
	msg->rssi = ap.rssi;

	msg->command = Command_Type::FULL_CONFIG;

	msg->layer = static_cast<uint8_t>(Node::layer());
	Node::parent(&msg->parent);

	memcpy(&msg->id, &cfg.mesh_id.addr, 6);
	Node::mac_ap(&msg->mac_ap);
	msg->channel_config = cfg.channel;
	msg->channel_conn = ap.primary;
	msg->is_root = Node::is_root();

	int route_table_size = 0;
	esp_mesh_get_routing_table((mesh_addr_t *)(tx_buf + sizeof(Response_Full_Config)),
									   CONFIG_MESH_ROUTE_TABLE_SIZE * 6, &route_table_size);

	make_header(&msg->header, Message_Type::COMMAND, sizeof(Response_Full_Config)
														- sizeof(Node_Header)
														+ route_table_size * 6);

	return msg->header.size + sizeof(Node_Header);
}

static void command_message(Node_Header* header, uint8_t* payload){
	if(header->size < sizeof(Command_Type))
	{
		ESP_LOGW(TAG, "Data message too small (%u)", header->size);
		return;
	}

	Message_Command* comm = reinterpret_cast<Message_Command*>(payload);
	switch(comm->type)
	{
		case Command_Type::ROUTE_TABLE:
			ESP_LOGI(TAG, "[COMMAND] Router table");
			send_route_table();
			break;
		case Command_Type::CONFIG:
			ESP_LOGI(TAG, "[COMMAND] Config");
			send_config();
			break;
		case Command_Type::STATUS:
			ESP_LOGI(TAG, "[COMMAND] Status");
			send_status();
			break;
		case Command_Type::FULL_CONFIG:
			ESP_LOGI(TAG, "[COMMAND] Full config");
			send_full_config();
			break;
		case Command_Type::REBOOT:
			ESP_LOGI(TAG, "[COMMAND] Reboot");
			esp_restart();
			break;
		case Command_Type::INFO:
			ESP_LOGE(TAG, "[COMMAND] Info: module should never received command type INFO");
			break;
		default:
//			ESP_LOGI(TAG, "Command not defined [%X]", comm->command);
			break;
	}
}

}//Mesh
