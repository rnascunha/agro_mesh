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
#include "coap_engine.hpp"
#include "messages/send.hpp"

#define TAG							"AGRO_MESH"

#define CONFIG_MESH_AP_PASSWD		"MAP_PASSWD"

static const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};

static esp_netif_t *netif_sta = NULL;
static bool ds_state_ = false;
extern bool coap_engine_started;

static bool init_packets_sended = false;

extern engine coap_engine;

esp_err_t coap_mesh_start(void) noexcept
{
    if (!coap_engine_started)
    {
    	coap_engine_started = true;

        xTaskCreate(coap_te_engine, "MPRX", 3072, NULL, 5, NULL);
        xTaskCreate(coap_send_main, "MPTX", 3072, NULL, 5, NULL);
    }
    else
    {
    	CoAP::Error ec;
		send_route(coap_engine, CoAP::Message::type::nonconfirmable, ec);
		if(ec)
		{
			ESP_LOGE(TAG, "Send route[%d/%s]", ec.value(), ec.message());
		}
    }
    return ESP_OK;
}

static void send_init_packtes()
{
	CoAP::Error ec;
	send_full_config(coap_engine, CoAP::Message::type::nonconfirmable, ec);
	if(ec)
	{
		ESP_LOGE(TAG, "Send full error[%d/%s]", ec.value(), ec.message());
	}
	ec.clear();

	send_config(coap_engine, CoAP::Message::type::nonconfirmable, ec);
	if(ec)
	{
		ESP_LOGE(TAG, "ERROR sending config [%d/%s]...", ec.value(), ec.message());
	}
	ec.clear();

	send_board_config(coap_engine, CoAP::Message::type::nonconfirmable, ec);
	if(ec)
	{
		ESP_LOGE(TAG, "ERROR sending board config [%d/%s]...", ec.value(), ec.message());
	}
}

/**
 * IP event handler.
 *
 * When receive a IP (i.e, is root), initalize proxy forward task
 */
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data) noexcept
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    ESP_LOGI(TAG, "<IP_EVENT_STA_GOT_IP>IP:" IPSTR, IP2STR(&event->ip_info.ip));
	esp_mesh_post_toDS_state(true);
	xTaskCreate(coap_forward_proxy, "root", 3072, NULL, 5, NULL);
}

/**
 * Mesh event handler.
 */
static void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data) noexcept
{
    switch (event_id)
    {
    	case MESH_EVENT_CHILD_CONNECTED: {
            mesh_event_child_connected_t *child_connected = (mesh_event_child_connected_t *)event_data;
            ESP_LOGI(TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, " MACSTR "",
                     child_connected->aid,
                     MAC2STR(child_connected->mac));

            CoAP::Error ec;
            send_route(coap_engine, CoAP::Message::type::nonconfirmable, ec);
            if(ec)
			{
				ESP_LOGE(TAG, "Send route[%d/%s]", ec.value(), ec.message());
			}
        }
        break;
    	case MESH_EVENT_CHILD_DISCONNECTED: {
			mesh_event_child_disconnected_t *child_disconnected = (mesh_event_child_disconnected_t *)event_data;
			ESP_LOGI(TAG, "<MESH_EVENT_CHILD_DISCONNECTED>aid:%d, " MACSTR "",
					 child_disconnected->aid,
					 MAC2STR(child_disconnected->mac));

			CoAP::Error ec;
			send_route(coap_engine, CoAP::Message::type::nonconfirmable, ec);
			if(ec)
			{
				ESP_LOGE(TAG, "Send route[%d/%s]", ec.value(), ec.message());
			}

		}
    	break;
		case MESH_EVENT_PARENT_CONNECTED: {
			/**
			 * Initialize tasks and DHCP if root
			 */
			if (esp_mesh_is_root())
			{
				esp_netif_dhcpc_start(netif_sta);
			}
			coap_mesh_start();
		}
		break;
		case MESH_EVENT_TODS_STATE: {
			mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
			ESP_LOGI(TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
			ds_state_ = *toDs_state ? true : false;
			if(!init_packets_sended && ds_state_)
			{
		        send_init_packtes();
		        init_packets_sended = true;
			}
		}
		break;
		default:
			break;
    }
}

bool ds_state() noexcept
{
	return ds_state_;
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
