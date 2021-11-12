#include "esp_log.h"
#include "coap_engine.hpp"

/**
 * Used at resources
 */
const char* RESOURCE_TAG = "RESOURCE";

static constexpr const char* COAP_TAG = "COAP";

/**
 * CoAP-te engine instantiation
 *
 * We prefer to instantiate the engine globally to not be limited by the
 * app_main stack size.
 *
 * If you prefer to instantiate locally at the main task (or any other task), depending
 * on the packet size/transaction number you choose, it can overflow the task memory.
 * Keep a eye on it.
 *
 * The main task memory size can be changed at:
 * > 'idf.py menuconfig' > 'Component config' > 'Common ESP-related' > 'Main task size'
 */
engine coap_engine{mesh_socket{},
					CoAP::Message::message_id{(unsigned)CoAP::random_generator()}};

extern engine::resource_node res_rtc_time;
extern engine::resource_node res_fuse_time;
extern engine::resource_node res_uptime;

extern engine::resource_node res_ac_load;
extern engine::resource_node res_ac_load1;
extern engine::resource_node res_ac_load2;
extern engine::resource_node res_ac_load3;

extern engine::resource_node res_net_rssi;
extern engine::resource_node res_net_waive_root;
extern engine::resource_node res_net_parent;
extern engine::resource_node res_net_config;
extern engine::resource_node res_net_full_config;
extern engine::resource_node res_net_route;
//extern engine::resource_node res_net_server;

extern engine::resource_node res_led;

extern engine::resource_node res_core;

extern engine::resource_node res_vesion;
extern engine::resource_node res_reset_reason;
extern engine::resource_node res_reboot;

extern engine::resource_node res_packet_sensors;
extern engine::resource_node res_packet_board;

extern engine::resource_node res_ota;
extern engine::resource_node res_job;
extern engine::resource_node res_app;

engine::resource_node res_well_knwon{".well-known"};
engine::resource_node res_net{"net"};
engine::resource_node res_packet{"packet"};

void init_coap_resources() noexcept
{
	ESP_LOGI(COAP_TAG, "Initializing CoAP Engine");

	ESP_LOGD(COAP_TAG, "Adding resources...");

	res_ac_load.add_child(res_ac_load1, res_ac_load2, res_ac_load3);
	res_net.add_child(res_net_rssi, res_net_waive_root, res_net_parent,
						res_net_config, res_net_full_config, res_net_route);//, res_net_server);
	res_well_knwon.add_child(res_core);
	res_packet.add_child(res_packet_board, res_packet_sensors);

	coap_engine.root_node().add_child(
				res_uptime,
				res_reboot,
				res_reset_reason,
				res_rtc_time,
				res_fuse_time,
				res_ac_load,
				res_led,
				res_net,
				res_vesion,
				res_packet,
				res_ota,
				res_job,
				res_app,
				res_well_knwon);
}

