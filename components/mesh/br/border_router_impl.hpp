#ifndef MESH_BORDER_ROUTER_APP_IMPL_HPP__
#define MESH_BORDER_ROUTER_APP_IMPL_HPP__

#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mesh/node.hpp"
#include "mesh/types.hpp"
#include "mesh/br_types.hpp"

#include <stdio.h>

#include <utility>      // std::move

#include "sys/errno.h"	//errno


namespace Mesh{

static constexpr const char* TAG = "MESH_BR";

#define MESH_BR_EXTERNAL_TASK_NAME		"mesh_br_external"
#define MESH_BR_EXTERNAL_TASK_SIZE		3072
#define MESH_BR_EXTERNAL_TASK_PRIORITY	5

#define MESH_BR_SOCKET_TASK_NAME		"mesh_br_socket"
#define MESH_BR_SOCKET_TASK_SIZE		3072
#define MESH_BR_SOCKET_TASK_PRIORITY	5

#define MESH_BORDER_ROUTER_DELAY_MS		1000

#define TX_SIZE			1024
static uint8_t tx_br_buf[TX_SIZE] = { 0, };

static volatile bool is_connecting = false;

extern void data_message(BR_Header* header, uint8_t* payload);
extern void command_message(BR_Header* header, uint8_t* payload);

template<bool StartRecvTODS>
static void connect_task(void* arg);

template<bool StartRecvTODS>
static void external_net_task(void *arg);

template<bool StartRecvTODS>
static void br_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);
template<bool StartRecvTODS>
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data);

template<bool StartRecvTODS>
Border_Router<StartRecvTODS>::Border_Router(const char* ip, uint16_t port)
	: socket_(ip, port){}

template<bool StartRecvTODS>
Border_Router<StartRecvTODS>::Border_Router(TCP_Client&& socket)
	: socket_(std::move(socket)){}

template<bool StartRecvTODS>
void Border_Router<StartRecvTODS>::init(){
    esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler<StartRecvTODS>, this);
    esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &br_event_handler<StartRecvTODS>, this);
}

template<bool StartRecvTODS>
void Border_Router<StartRecvTODS>::deinit(){
	esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler<StartRecvTODS>);
	esp_event_handler_unregister(MESH_EVENT, ESP_EVENT_ANY_ID, &br_event_handler<StartRecvTODS>);
}

template<bool StartRecvTODS>
void Border_Router<StartRecvTODS>::stop(){
	close_socket();
}

template<bool StartRecvTODS>
void Border_Router<StartRecvTODS>::open(){
	if(!is_connecting)
	{
		is_connecting = true;
		xTaskCreatePinnedToCore(connect_task<StartRecvTODS>,
				MESH_BR_SOCKET_TASK_NAME,
				MESH_BR_SOCKET_TASK_SIZE, this,
				MESH_BR_SOCKET_TASK_PRIORITY, NULL, tskNO_AFFINITY);
	}
}

template<bool StartRecvTODS>
void Border_Router<StartRecvTODS>::reconnect()
{
	close_socket();
	if(Node::is_root()) open();
}

template<bool StartRecvTODS>
void Border_Router<StartRecvTODS>::close_socket()
{
	Node::post_toDS_state(false);
	socket_.close();
}

template<bool StartRecvTODS>
TCP_Client::err Border_Router<StartRecvTODS>::open_socket()
{
	TCP_Client::err ret =  this->socket_.open();
	if(ret == TCP_Client::success)
	{
		struct timeval to;
		to.tv_sec = 3;
		to.tv_usec = 0;
		this->socket_.option(SO_RCVTIMEO, &to, sizeof(struct timeval));

		this->socket_.keep_alive(60, 5, 3);
	}

	return ret;
}

template<bool StartRecvTODS>
int Border_Router<StartRecvTODS>::write_socket(const void* data, std::size_t size, int flags /* = 0 */)
{
	return this->socket_.write(data, size, flags);
}

template<bool StartRecvTODS>
int Border_Router<StartRecvTODS>::read_socket(void* data, std::size_t size, int flags /* = 0 */)
{
	return this->socket_.read(data, size, flags);
}

template<bool StartRecvTODS>
static void connect_task(void* arg){
	ESP_LOGD(TAG, "BR connect_task init");

	Mesh::Border_Router<StartRecvTODS>* router = reinterpret_cast<Mesh::Border_Router<StartRecvTODS>*>(arg);

	while(router->open_socket() != TCP_Client::success && Node::is_root()){
		ESP_LOGE(TAG, "Error opening socket");
		vTaskDelay(MESH_BORDER_ROUTER_DELAY_MS / portTICK_RATE_MS);
	}
	is_connecting = false;

	if(Node::is_root()){
		Node::post_toDS_state(true);
		xTaskCreatePinnedToCore(external_net_task<StartRecvTODS>,
					MESH_BR_EXTERNAL_TASK_NAME, MESH_BR_EXTERNAL_TASK_SIZE,
					router, MESH_BR_EXTERNAL_TASK_PRIORITY, NULL, tskNO_AFFINITY);
	} else
		router->close_socket();

	ESP_LOGD(TAG, "BR connect_task fin");
    vTaskDelete(NULL);
}

template<bool StartRecvTODS>
static void external_net_task(void *arg)
{
	ESP_LOGD(TAG, "BR external_net_task init");
	Mesh::Border_Router<StartRecvTODS>* router = static_cast<Mesh::Border_Router<StartRecvTODS>*>(arg);

	//Receiving from external net
	uint8_t* data = tx_br_buf;
	while(Node::is_root()){
		int err = router->read_socket(data, TX_SIZE);
		if(err > 0)
		{
			BR_Header* header = reinterpret_cast<BR_Header*>(data);
			switch(header->type)
			{
				case BR_Message_Type::DATA:
					data_message(header, data + sizeof(BR_Header));
					break;
				case BR_Message_Type::COMMAND_BR:
					command_message(header, data + sizeof(BR_Header));
					break;
				default:
//					ESP_LOGE(TAG, "ERROR! Message type not defined [%d/%u]", header->type, header->size);
					break;
			}
		} else {
			if(err == 0 || errno != TCP_Client::Errno::Would_Block){
				ESP_LOGE(TAG, "Error receivend socket %d/%d", err, errno);
				break;
			}
		}
	}
	router->reconnect();

	ESP_LOGI(TAG, "BR external_net_task fin");
    vTaskDelete(NULL);
}

template<bool StartRecvTODS>
static void br_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
	Mesh::Border_Router<StartRecvTODS>* router = reinterpret_cast<Mesh::Border_Router<StartRecvTODS>*>(arg);

	switch(event_id){
//		case MESH_EVENT_STARTED:
			//	mesh is started
//			break;
//		case MESH_EVENT_STOPPED:
			//	mesh is stopped
//			break;
//		case MESH_EVENT_CHANNEL_SWITCH:
			//	channel switch
//			break;
//		case MESH_EVENT_CHILD_CONNECTED:
			//	a child is connected on softAP interface
//			break;
//		case MESH_EVENT_CHILD_DISCONNECTED:
			//	a child is disconnected on softAP interface
//			break;
//		case MESH_EVENT_ROUTING_TABLE_ADD:
			//	routing table is changed by adding newly joined children
//			break;
//		case MESH_EVENT_ROUTING_TABLE_REMOVE:
			//	routing table is changed by removing leave children
//			break;
		case MESH_EVENT_PARENT_CONNECTED:
			//	parent is connected on station interface
	        if (Node::is_root()){
	            tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
	            router->start();
	        }
	        break;
//		case MESH_EVENT_PARENT_DISCONNECTED:
			//	parent is disconnected on station interface
//			break;
//		case MESH_EVENT_NO_PARENT_FOUND:
			//	no parent found
//			break;
//		case MESH_EVENT_LAYER_CHANGE:
			//	layer changes over the mesh network
//			break;
//		case MESH_EVENT_TODS_STATE:
			//	state represents whether the root is able to access external IP network
//			break;
//		case MESH_EVENT_VOTE_STARTED:
			//	the process of voting a new root is started either by children or by the root
//			break;
//		case MESH_EVENT_VOTE_STOPPED:
			//	the process of voting a new root is stopped
//			break;
//		case MESH_EVENT_ROOT_ADDRESS:
			//	the root address is obtained. It is posted by mesh stack automatically.
//			break;
//		case MESH_EVENT_ROOT_SWITCH_REQ:
			//	root switch request sent from a new voted root candidate
//			break;
//		case MESH_EVENT_ROOT_SWITCH_ACK:
			//	root switch acknowledgment responds the above request sent from current root
//			break;
//		case MESH_EVENT_ROOT_ASKED_YIELD:
			//	the root is asked yield by a more powerful existing root. If self organized is disabled and this device is specified to be a root by users, users should set a new parent for this device. if self organized is enabled, this device will find a new parent by itself, users could ignore this event.
//			break;
//		case MESH_EVENT_ROOT_FIXED:
			//	when devices join a network, if the setting of Fixed Root for one device is different from that of its parent, the device will update the setting the same as its parent’s. Fixed Root Setting of each device is variable as that setting changes of the root.
//			break;
//		case MESH_EVENT_SCAN_DONE:
			//	if self-organized networking is disabled, user can call esp_wifi_scan_start() to trigger this event, and add the corresponding scan done handler in this event.
//			break;
//		case MESH_EVENT_NETWORK_STATE:
			//	network state, such as whether current mesh network has a root.
//			break;
//		case MESH_EVENT_STOP_RECONNECTION:
			//	the root stops reconnecting to the router and non-root devices stop reconnecting to their parents.
//			break;
//		case MESH_EVENT_FIND_NETWORK:
			//	when the channel field in mesh configuration is set to zero, mesh stack will perform a full channel scan to find a mesh network that can join, and return the channel value after finding it.
//			break;
//		case MESH_EVENT_ROUTER_SWITCH:
			//	if users specify BSSID of the router in mesh configuration, when the root connects to another router with the same SSID, this event will be posted and the new router information is attached.
//			break;
//		case MESH_EVENT_PS_PARENT_DUTY:
			//	parent duty
//			break;
//		case MESH_EVENT_PS_CHILD_DUTY:
//			break;
		default:
			break;
	}
}

template<bool StartRecvTODS>
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
	Mesh::Border_Router<StartRecvTODS>* router = static_cast<Mesh::Border_Router<StartRecvTODS>*>(arg);
	switch(event_id)
	{
		case IP_EVENT_STA_GOT_IP:
		{
			ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
			ESP_LOGI(TAG, "<IP_EVENT_STA_GOT_IP>IP:%s", ip4addr_ntoa((const ip4_addr_t *)(&event->ip_info.ip)));
			router->open();
		}
			break;
		case IP_EVENT_STA_LOST_IP:
			router->close_socket();
			break;
	}
}

}//Mesh

#endif /* MESH_BORDER_ROUTER_APP_IMPL_HPP__ */
