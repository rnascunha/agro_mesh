#include "esp_log.h"
#include "esp_err.h"
#include "esp_mesh.h"

#include "mesh/node.hpp"
#include "mesh/node_message.hpp"

namespace Mesh{

#define TAG		"MESH_NODE"

#define MESH_NODE_BUFFER_SIZE	1024
static uint8_t rx_buf[MESH_NODE_BUFFER_SIZE];

extern bool node_started;

void __attribute__((weak))
node_receive_task(void*)
{
	ESP_LOGI(TAG, "NODE node_receive_task init");

    esp_err_t err;
    mesh_addr_t from;
    mesh_data_t data;
    int flag = 0;
    data.data = rx_buf;

    while(node_started)
    {
        data.size = MESH_NODE_BUFFER_SIZE;

        err = esp_mesh_recv(&from, &data, portMAX_DELAY, &flag, NULL, 0);
        if (err == ESP_OK && data.size)
        {
        	if(flag == MESH_DATA_FROMDS)
        		node_fromds_message(&from, &data, Node::fromds_cb_arg());
			else
				node_tods_message(&from, &data, Node::tods_cb_arg());
        } else
        	ESP_LOGE(TAG, "[size:%u] Error internal receiving: [%d/%s]",
        			data.size, err, esp_err_to_name(err));
    }
	ESP_LOGI(TAG, "NODE node_receive_task fin");
    vTaskDelete(NULL);
}

void __attribute__((weak))
node_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
	switch(event_id){
		case MESH_EVENT_STARTED:
			//	mesh is started
			break;
		case MESH_EVENT_STOPPED:
			//	mesh is stopped
			break;
		case MESH_EVENT_CHANNEL_SWITCH:
			//	channel switch
			send_config();
			break;
		case MESH_EVENT_CHILD_CONNECTED:
			//	a child is connected on softAP interface
//			send_route_table();
			send_info("Child connected");
			break;
		case MESH_EVENT_CHILD_DISCONNECTED:
			//	a child is disconnected on softAP interface
//			send_route_table();
			send_info("Child disconnected");
			break;
		case MESH_EVENT_ROUTING_TABLE_ADD:
			//	routing table is changed by adding newly joined children
			send_route_table();
			break;
		case MESH_EVENT_ROUTING_TABLE_REMOVE:
			//	routing table is changed by removing leave children
			send_route_table();
			break;
		case MESH_EVENT_PARENT_CONNECTED:
			//	parent is connected on station interface
			Node::start();
			if(Node::ds_state())
				send_full_config();
	        break;
		case MESH_EVENT_PARENT_DISCONNECTED:
			//	parent is disconnected on station interface
			break;
		case MESH_EVENT_NO_PARENT_FOUND:
			//	no parent found
			break;
		case MESH_EVENT_LAYER_CHANGE:
			//	layer changes over the mesh network
			send_route_table();
			break;
		case MESH_EVENT_TODS_STATE:
			//	state represents whether the root is able to access external IP network
			Node::ds_state(*reinterpret_cast<mesh_event_toDS_state_t*>(event_data)
									== MESH_TODS_UNREACHABLE ? false : true);
			if(Node::ds_state()) send_full_config();
			break;
		case MESH_EVENT_VOTE_STARTED:
			//	the process of voting a new root is started either by children or by the root
			send_info("Vote started");
			break;
		case MESH_EVENT_VOTE_STOPPED:
			//	the process of voting a new root is stopped
			send_info("Vote stoped");
			break;
		case MESH_EVENT_ROOT_ADDRESS:
			//	the root address is obtained. It is posted by mesh stack automatically.
			break;
		case MESH_EVENT_ROOT_SWITCH_REQ:
			//	root switch request sent from a new voted root candidate
			break;
		case MESH_EVENT_ROOT_SWITCH_ACK:
			//	root switch acknowledgment responds the above request sent from current root
			break;
		case MESH_EVENT_ROOT_ASKED_YIELD:
			//	the root is asked yield by a more powerful existing root. If self organized is disabled and this device is specified to be a root by users, users should set a new parent for this device. if self organized is enabled, this device will find a new parent by itself, users could ignore this event.
			break;
		case MESH_EVENT_ROOT_FIXED:
			//	when devices join a network, if the setting of Fixed Root for one device is different from that of its parent, the device will update the setting the same as its parent’s. Fixed Root Setting of each device is variable as that setting changes of the root.
			break;
		case MESH_EVENT_SCAN_DONE:
			//	if self-organized networking is disabled, user can call esp_wifi_scan_start() to trigger this event, and add the corresponding scan done handler in this event.
			break;
		case MESH_EVENT_NETWORK_STATE:
			//	network state, such as whether current mesh network has a root.
			break;
		case MESH_EVENT_STOP_RECONNECTION:
			//	the root stops reconnecting to the router and non-root devices stop reconnecting to their parents.
			break;
		case MESH_EVENT_FIND_NETWORK:
			//	when the channel field in mesh configuration is set to zero, mesh stack will perform a full channel scan to find a mesh network that can join, and return the channel value after finding it.
			break;
		case MESH_EVENT_ROUTER_SWITCH:
			//	if users specify BSSID of the router in mesh configuration, when the root connects to another router with the same SSID, this event will be posted and the new router information is attached.
			break;
		case MESH_EVENT_PS_PARENT_DUTY:
			//	parent duty
			break;
		case MESH_EVENT_PS_CHILD_DUTY:
			break;
		default:
			break;
	}
}

void wifi_default_init(esp_netif_t* netif_sta)
{
	/*  tcpip initialization */
	esp_netif_init();
	/*  event initialization */
	esp_event_loop_create_default();
	/*  create network interfaces for mesh (only station instance saved for further manipulation, soft AP instance ignored */
	esp_netif_create_default_wifi_mesh_netifs(&netif_sta, NULL);

	/*  wifi initialization */
	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&config);

	esp_wifi_set_storage(WIFI_STORAGE_FLASH);

	esp_wifi_start();
}

}//Mesh
