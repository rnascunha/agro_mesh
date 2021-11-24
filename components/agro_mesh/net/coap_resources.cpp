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

#if CONFIG_ENABLE_COAP_RESOURCES

#if CONFIG_COAP_RESOURCE_UPTIME
extern engine::resource_node res_uptime;
#endif /* CONFIG_COAP_RESOURCE_UPTIME */

#if CONFIG_COAP_RESOURCE_NET_RSSI
extern engine::resource_node res_net_rssi;
#endif /* CONFIG_COAP_RESOURCE_NET_RSSI */
#if CONFIG_COAP_RESOURCE_NET_WAIVE_ROOT
extern engine::resource_node res_net_waive_root;
#endif /* CONFIG_COAP_RESOURCE_NET_RSSI */
#if CONFIG_COAP_RESOURCE_NET_PARENT
extern engine::resource_node res_net_parent;
#endif /* CONFIG_COAP_RESOURCE_NET_PARENT */
#if CONFIG_COAP_RESOURCE_NET_CONFIG
extern engine::resource_node res_net_config;
#endif /* CONFIG_COAP_RESOURCE_NET_CONFIG */
#if CONFIG_COAP_RESOURCE_NET_FULL_CONFIG
extern engine::resource_node res_net_full_config;
#endif /* CONFIG_COAP_RESOURCE_NET_FULL_CONFIG */
#if CONFIG_COAP_RESOURCE_NET_ROUTE
extern engine::resource_node res_net_route;
#endif /* CONFIG_COAP_RESOURCE_NET_ROUTE */

#if CONFIG_COAP_RESOURCE_LED
extern engine::resource_node res_led;
#endif /* CONFIG_COAP_RESOURCE_LED */

#if CONFIG_COAP_RESOURCE_VERSION
extern engine::resource_node res_vesion;
#endif /* CONFIG_COAP_RESOURCE_VERSION */

#if CONFIG_COAP_RESOURCE_RESET_REASON
extern engine::resource_node res_reset_reason;
#endif /* CONFIG_COAP_RESOURCE_RESET_REASON */
#if CONFIG_COAP_RESOURCE_REBOOT
extern engine::resource_node res_reboot;
#endif /* CONFIG_COAP_RESOURCE_REBOOT */

#if CONFIG_COAP_RESOURCE_DISCOVERY
engine::resource_node res_well_knwon{".well-known"};
extern engine::resource_node res_core;
#endif /* CONFIG_COAP_RESOURCE_DISCOVERY */

#if CONFIG_COAP_RESOURCE_NET_RSSI || \
	CONFIG_COAP_RESOURCE_NET_WAIVE_ROOT || \
	CONFIG_COAP_RESOURCE_NET_PARENT || \
	CONFIG_COAP_RESOURCE_NET_ROUTE || \
	CONFIG_COAP_RESOURCE_NET_CONFIG || \
	CONFIG_COAP_RESOURCE_NET_FULL_CONFIG
engine::resource_node res_net{"net"};
#endif

#endif /* CONFIG_ENABLE_COAP_RESOURCES */

#if CONFIG_ENABLE_OTA_SUPPORT == 1
extern engine::resource_node res_ota;
#endif /* CONFIG_ENABLE_OTA_SUPPORT == 1 */

#if CONFIG_ENABLE_APP_SUPPORT
extern engine::resource_node res_app;
#endif /* CONFIG_ENABLE_APP_SUPPORT */

#if CONFIG_ENABLE_DEVICE_CLOCK
extern engine::resource_node res_rtc_time;
extern engine::resource_node res_fuse_time;
#endif /* CONFIG_ENABLE_DEVICE_CLOCK */

#if CONFIG_ENABLE_JOBS_SUPPORT && CONFIG_ENABLE_DEVICE_CLOCK
extern engine::resource_node res_job;
#endif /* CONFIG_ENABLE_JOBS_SUPPORT && CONFIG_ENABLE_DEVICE_CLOCK*/

void init_coap_resources() noexcept
{
	ESP_LOGD(COAP_TAG, "Initializing CoAP Engine");

#if CONFIG_ENABLE_COAP_RESOURCES

#if CONFIG_COAP_RESOURCE_NET_RSSI
	res_net.add_child(res_net_rssi);
#endif /* CONFIG_COAP_RESOURCE_NET_RSSI */
#if CONFIG_COAP_RESOURCE_NET_WAIVE_ROOT
	res_net.add_child(res_net_waive_root);
#endif /* CONFIG_COAP_RESOURCE_NET_WAIVE_ROOT */
#if CONFIG_COAP_RESOURCE_NET_PARENT
	res_net.add_child(res_net_parent);
#endif /* CONFIG_COAP_RESOURCE_NET_PARENT */
#if CONFIG_COAP_RESOURCE_NET_CONFIG
	res_net.add_child(res_net_config);
#endif /* CONFIG_COAP_RESOURCE_NET_CONFIG */
#if CONFIG_COAP_RESOURCE_NET_FULL_CONFIG
	res_net.add_child(res_net_full_config);
#endif /* CONFIG_COAP_RESOURCE_NET_FULL_CONFIG */
#if CONFIG_COAP_RESOURCE_NET_ROUTE
	res_net.add_child(res_net_route);
#endif /* CONFIG_COAP_RESOURCE_NET_ROUTE */

#if CONFIG_COAP_RESOURCE_DISCOVERY
	res_well_knwon.add_child(res_core);
#endif /* CONFIG_COAP_RESOURCE_DISCOVERY */

#if CONFIG_COAP_RESOURCE_UPTIME
	coap_engine.root_node().add_child(res_uptime);
#endif /* CONFIG_COAP_RESOURCE_UPTIME */
#if CONFIG_COAP_RESOURCE_REBOOT
	coap_engine.root_node().add_child(res_reboot);
#endif /* CONFIG_COAP_RESOURCE_REBOOT */
#if CONFIG_COAP_RESOURCE_RESET_REASON
	coap_engine.root_node().add_child(res_reset_reason);
#endif /* CONFIG_COAP_RESOURCE_RESET_REASON */

#if CONFIG_COAP_RESOURCE_LED
	coap_engine.root_node().add_child(res_led);
#endif /* CONFIG_COAP_RESOURCE_LED */

#if CONFIG_COAP_RESOURCE_NET_RSSI || \
	CONFIG_COAP_RESOURCE_NET_WAIVE_ROOT || \
	CONFIG_COAP_RESOURCE_NET_PARENT || \
	CONFIG_COAP_RESOURCE_NET_ROUTE || \
	CONFIG_COAP_RESOURCE_NET_CONFIG || \
	CONFIG_COAP_RESOURCE_NET_FULL_CONFIG
	coap_engine.root_node().add_child(res_net);
#endif
#if CONFIG_COAP_RESOURCE_VERSION
	coap_engine.root_node().add_child(res_vesion);
#endif /* CONFIG_COAP_RESOURCE_VERSION */
//				, res_job
#if	CONFIG_COAP_RESOURCE_DISCOVERY
	coap_engine.root_node().add_child(res_well_knwon);
#endif /* CONFIG_COAP_RESOURCE_DISCOVERY */

#endif /* CONFIG_ENABLE_COAP_RESOURCES */

#if CONFIG_ENABLE_OTA_SUPPORT
	coap_engine.root_node().add_child(res_ota);
#endif /* CONFIG_ENABLE_OTA_SUPPORT */

#if CONFIG_ENABLE_APP_SUPPORT
	coap_engine.root_node().add_child(res_app);
#endif /* CONFIG_ENABLE_APP_SUPPORT */

#if CONFIG_ENABLE_DEVICE_CLOCK
	coap_engine.root_node().add_child(res_rtc_time);
	coap_engine.root_node().add_child(res_fuse_time);
#endif /* CONFIG_ENABLE_DEVICE_CLOCK */

#if CONFIG_ENABLE_JOBS_SUPPORT && CONFIG_ENABLE_DEVICE_CLOCK
	coap_engine.root_node().add_child(res_job);
#endif /* CONFIG_ENABLE_JOBS_SUPPORT && CONFIG_ENABLE_DEVICE_CLOCK*/

}
