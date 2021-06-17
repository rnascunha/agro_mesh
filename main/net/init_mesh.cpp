#include <string.h>

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"

#include "esp_mesh.h"
#include "esp_mesh_internal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tasks.hpp"

#define TAG							"AGRO_MESH"

#define CONFIG_MESH_AP_PASSWD		"MAP_PASSWD"

static const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};

static esp_netif_t *netif_sta = NULL;
extern bool coap_engine_started;

esp_err_t coap_mesh_start(void)
{
    if (!coap_engine_started)
    {
    	coap_engine_started = true;

        xTaskCreate(coap_te_engine, "MPRX", 3072, NULL, 5, NULL);
#ifdef JUST_EXAMPLE_NEVER_RUN
        xTaskCreate(coap_send_main, "MPTX", 3072, NULL, 5, NULL);
#endif /* JUST_EXAMPLE_NEVER_RUN */
    }
    return ESP_OK;
}

/**
 * IP event handler.
 *
 * When receive a IP (i.e, is root), initalize proxy forward task
 */
void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    ESP_LOGI(TAG, "<IP_EVENT_STA_GOT_IP>IP:" IPSTR, IP2STR(&event->ip_info.ip));
	esp_mesh_post_toDS_state(true);
	xTaskCreate(coap_forward_proxy, "root", 3072, NULL, 5, NULL);
}

/**
 * Mesh event handler.
 */
void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    switch (event_id)
    {
		break;
		case MESH_EVENT_PARENT_CONNECTED: {
			/**
			 * Initialize tasks and DHCP if root
			 */
			if (esp_mesh_is_root()) {
				esp_netif_dhcpc_start(netif_sta);
			}
			coap_mesh_start();
		}
		break;
		case MESH_EVENT_TODS_STATE: {
			mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
			ESP_LOGI(TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
		}
		break;
		default:
			break;
    }
}


void init_mesh() noexcept
{
	ESP_LOGI(TAG, "Initializing Mesh network");
	/**
	 * ESP32 mesh initialization
	 */
	/*  tcpip initialization */
	esp_netif_init();
	/*  event initialization */
	esp_event_loop_create_default();
	/*  create network interfaces for mesh (only station instance saved for further manipulation, soft AP instance ignored */
	esp_netif_create_default_wifi_mesh_netifs(&netif_sta, NULL);
	/*  wifi initialization */
	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&config);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL);
	esp_wifi_set_storage(WIFI_STORAGE_FLASH);
	esp_wifi_start();
	/*  mesh initialization */
	esp_mesh_init();
	esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL);
	/*  set mesh topology */
	esp_mesh_set_topology(MESH_TOPO_TREE);
	/*  set mesh max layer according to the topology */
	esp_mesh_set_max_layer(6);
	esp_mesh_set_vote_percentage(1);
	esp_mesh_set_xon_qsize(128);
	/* Disable mesh PS function */
	esp_mesh_disable_ps();
	esp_mesh_set_ap_assoc_expire(10);

	mesh_cfg_t cfg = {};
	cfg.crypto_funcs = &g_wifi_default_mesh_crypto_funcs;
	esp_mesh_allow_root_conflicts(false);
	/* mesh ID */
	memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);
	/* router */
	cfg.channel = 0;
	cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
	memcpy((uint8_t *) &cfg.router.ssid, CONFIG_MESH_ROUTER_SSID, cfg.router.ssid_len);
	memcpy((uint8_t *) &cfg.router.password, CONFIG_MESH_ROUTER_PASSWD,
		   strlen(CONFIG_MESH_ROUTER_PASSWD));
	/* mesh softAP */
	esp_mesh_set_ap_authmode(WIFI_AUTH_WPA2_PSK);
	cfg.mesh_ap.max_connection = 6;
	memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
		   strlen(CONFIG_MESH_AP_PASSWD));
	esp_mesh_set_config(&cfg);
	/* mesh start */
	esp_mesh_start();

	ESP_LOGI(TAG, "mesh starts successfully, heap:%d, %s<%d>%s, ps:%d\n",  esp_get_minimum_free_heap_size(),
	esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed",
	esp_mesh_get_topology(), esp_mesh_get_topology() ? "(chain)":"(tree)", esp_mesh_is_ps_enabled());
}
