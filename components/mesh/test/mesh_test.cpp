#include <string.h>

#include "sdkconfig.h"

#include "esp_event.h"
#include "esp_log.h"

#include "esp_mesh.h"

#include "esp_netif_types.h"

#include "tcp_client.hpp"

#include "mesh/border_router.hpp"
#include "mesh/node.hpp"
#include "mesh/types.hpp"

#include "nvs_flash.h"

/*******************************************************
 *                Variable Definitions
 *******************************************************/
static constexpr const char *TAG = "MESH_MAIN";
static esp_netif_t *netif_sta = NULL;

#define HOST_IP_ADDR	"192.168.0.9"
#define HOST_PORT		8089

#define USE_TODS_TASK	1

extern "C" void app_main(void)
{
	nvs_flash_init();
	Mesh::wifi_default_init(netif_sta);

	{
		Mesh::other_mesh_cfg_t other_cfg = {
			.topology = static_cast<esp_mesh_topology_t>(CONFIG_MESH_TOPOLOGY),
			.vote_percent = 1,
			.xon_queue = 128,
			.expire_sec = 10,
			.max_layer = CONFIG_MESH_MAX_LAYER,
			.enable_ps = false,
			.wifi_auth = static_cast<wifi_auth_mode_t>(CONFIG_MESH_AP_AUTHMODE),
			.allow_root_conflicts = false
		};

		mesh_cfg_t cfg = {
			.channel = CONFIG_MESH_CHANNEL,
			.allow_channel_switch = false,
			.mesh_id = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77},
			.router = {
				CONFIG_MESH_ROUTER_SSID,
				strlen(CONFIG_MESH_ROUTER_SSID),
				{0,0,0,0,0,0},	//bssid
				CONFIG_MESH_ROUTER_PASSWD,
				false	//allow_router_switch
			},
			.mesh_ap = {
				CONFIG_MESH_AP_PASSWD,
				CONFIG_MESH_AP_CONNECTIONS
			},
			.crypto_funcs = &g_wifi_default_mesh_crypto_funcs
		};
		Mesh::Node::init(cfg, other_cfg);
	}

#if USE_TODS_TASK == 1
	mesh_addr_t to;
	to.mip.ip4.addr = inet_addr(HOST_IP_ADDR);
	to.mip.port = htons(HOST_PORT);
	Mesh::Node::set_to_send(&to);
	Mesh::Border_Router<true>(TCP_Client(HOST_IP_ADDR, HOST_PORT)).init();
#else
	Mesh::Border_Router<false>(TCP_Client(HOST_IP_ADDR, HOST_PORT)).init();
#endif
	Mesh::Node::register_event(
    		MESH_EVENT_PARENT_CONNECTED,
			[](void *arg, esp_event_base_t event_base,
            	int32_t event_id, void *event_data){

    	mesh_addr_t mac, parent, id;
    	Mesh::Node::mac(&mac);
    	Mesh::Node::parent(&parent);
    	Mesh::Node::id(&id);

        ESP_LOGI(TAG, "%s[%d]:[" MACSTR "]([" MACSTR "]->[" MACSTR "])", Mesh::Node::topology() == MESH_TOPO_TREE
        																? "tree" : "chain",
																		Mesh::Node::layer(),
        																MAC2STR(id.addr),
        																MAC2STR(mac.addr),
    																	MAC2STR(parent.addr));
        ESP_LOGI(TAG, "is root: %s, is_ps: %s", Mesh::Node::is_root() ?
        											"true" : "false",
        										Mesh::Node::is_power_save() ?
        											"true" : "false");
    }, NULL);

    while(true) vTaskDelay(portMAX_DELAY);
}
