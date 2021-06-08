#include <stdio.h>

//#include "print_system_info.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "gpio.h"

#include "i2c_master.h"
#include "datetime.h"
#include "ds3231.hpp"

#include "onewire.h"
#include "dallas_temperature.h"

#include "pinout.hpp"
//#include "get_type_string.h"

static const char* TAG = "APP MAIN";

//#define I2C_FREQ_HZ		400000

extern "C" void app_main(void)
{
	ESP_LOGI(TAG, "Iniciando!");

	GPIO_Basic gpio_sensor(DS18B20_DATA);
	OneWire onewire(&gpio_sensor);
	Dallas_Temperature dallas(&onewire);

	GPIO_Basic led(ONBOARD_LED), out1(TRIAC_OUTPUT1);
	led.mode(GPIO_MODE_OUTPUT);
	out1.mode(GPIO_MODE_OUTPUT);

	led.write(0);
	out1.write(0);

	I2C_Master i2c(I2C_NUM_0, I2C_SCL, I2C_SCA, I2C_FAST_SPEED_HZ);
	i2c.init();

	DS3231 rtc(&i2c);
	rtc.begin();

	DateTime dt;
	//Hard code time
	dt.setUnixTime(1623109458);
	rtc.setDateTime(&dt);
	while(true){
		dallas.requestTemperatures();
		led.toggle();
		out1.toggle();
		rtc.getDateTime(&dt);

		printf("Time: %u/%u/%u %u:%u:%u %u | Temp(C): %f\n",
				dt.getDay(), dt.getMonth(), dt.getYear(),
				dt.getHour(), dt.getMinute(), dt.getSecond(),
				dt.dayOfWeek(),
				dallas.getTempCByIndex(0)
				);

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}


///****************************************************************
// * \brief Xerock Mesh example
// *
// * Firmware implements a mesh network at ESP32 devices. Devices
// * at newtwokt work as Nodes and (possibly) border router. As
// * border router devices will try to connect to a server outside
// * the mesh network, forwarding all messages from internal devices.
// * Messages data received from outside will be echoed back.
// *
// * This firmware was develop to be used with ESP Mesh Script at
// * https://rnascunha.github.io/xerock.
// *
// * \author Rafael Cunha
// * \date 01/2021
// */
//
///*****************************
// * Includes
// *****************************/
//
//#include <string.h>
//
//#include "sdkconfig.h"
//
//#include "esp_event.h"
//#include "esp_log.h"
//
//#include "esp_mesh.h"
//
//#include "esp_netif_types.h"
//
//#include "tcp_client.hpp"
//
//#include "mesh/border_router.hpp"
//#include "mesh/node.hpp"
//#include "mesh/types.hpp"
//#include "mesh/node_message.hpp"
//#include "mesh/node_types.hpp"
//
//#include "nvs_flash.h"
//
///*******************************************************
// *                Variable Definitions
// *******************************************************/
//static constexpr const char *TAG = "MESH_MAIN";
//static esp_netif_t *netif_sta = NULL;
//
//#define HOST_IP_ADDR	CONFIG_MESH_BR_SERVER_IP 		///< Server IP address
//#define HOST_PORT		CONFIG_MESH_BR_SERVER_PORT		///< Server port
//
//#ifdef CONFIG_MESH_TODS_ENABLE
//#define USE_TODS_TASK	1
//#else
//#define USE_TODS_TASK	0
//#endif /* CONFIG_MESH_TODS_ENABLE */
//
//#ifdef CONFIG_MESH_ALLOW_ROOT_CONFLICT
//#define ALLOW_ROOT_CONFLICT	true
//#else
//#define ALLOW_ROOT_CONFLICT	false
//#endif /* CONFIG_MESH_ALLOW_ROOT_CONFLICT */
//
//namespace Mesh{
//
///**
// * \brief Generate message that will be echoed
// *
// * Function fill buffer with header parameters and copy data
// * payload
// *
// * @param [in] h Header of received message
// * @param [out] buffer Buffer to fill reply message
// * @param [in] payload Payload of received message
// *
// * @return generated message size
// */
//static size_t echo_message(Node_Header* h, uint8_t* buffer, uint8_t* payload)
//{
//	Node_Header* header = reinterpret_cast<Node_Header*>(buffer);
//
//	memcpy(buffer + sizeof(Node_Header*), payload, h->size);
//	header->type = Message_Type::DATA;
//	header->version = node_version;
//	header->size = h->size;
//
//	return h->size + sizeof(Node_Header);
//}
//
///**
// * \brief Node data message handler
// *
// * This function is called every time the mesh node receives a
// * message of type data.
// *
// * \note This function will overwrite the default handler (that just prints
// * the message).
// *
// * @param [in] h Header of received message
// * @param [out] buffer Buffer to fill reply message
// * @param [in] payload Payload of received message
// * @param [in] arg Argument that (could be) set before call
// */
//void node_data_handler(Node_Header* header, uint8_t* payload, uint8_t* buffer, void* arg)
//{
//	mesh_data_t s_data;
//
//	s_data.data = buffer;
//	s_data.size = echo_message(header, buffer, payload);
//	s_data.proto = MESH_PROTO_BIN;
//	s_data.tos = MESH_TOS_P2P;
//
//	esp_err_t err = Node::to_send(&s_data);
//	if(err != ESP_OK)
//		ESP_LOGE(TAG, "[%X/%s] Error sending DATA to ROOT [%u]", err, esp_err_to_name(err), s_data.size);
//}
//
//}//Mesh
//
///**
// * Main
// */
//extern "C" void app_main(void)
//{
//	/**
//	 * Basic initializations
//	 */
//	nvs_flash_init();
//	Mesh::wifi_default_init(netif_sta);
//
//	{
//		/**
//		 * Setting configuration parameters
//		 */
//		Mesh::other_mesh_cfg_t other_cfg = {
//			//Setting topology (tree or chain)
//			.topology = static_cast<esp_mesh_topology_t>(CONFIG_MESH_TOPOLOGY),
//			.vote_percent = 1,
//			.xon_queue = 128,
//			.expire_sec = 10,
//			.max_layer = CONFIG_MESH_MAX_LAYER,
//			.enable_ps = false,
//			.wifi_auth = static_cast<wifi_auth_mode_t>(CONFIG_MESH_AP_AUTHMODE),
//			//Allowing more than one border router
//			.allow_root_conflicts = ALLOW_ROOT_CONFLICT
//		};
//
//		mesh_cfg_t cfg = {
//			.channel = CONFIG_MESH_CHANNEL,
//			.allow_channel_switch = false,
//			.mesh_id = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77},
//			.router = {
//				CONFIG_MESH_ROUTER_SSID,			///<	Local router SSID
//				strlen(CONFIG_MESH_ROUTER_SSID),
//				{0,0,0,0,0,0},	//bssid
//				CONFIG_MESH_ROUTER_PASSWD,			///<	Local router password
//				false	//allow_router_switch
//			},
//			.mesh_ap = {
//				CONFIG_MESH_AP_PASSWD,
//				CONFIG_MESH_AP_CONNECTIONS
//			},
//			.crypto_funcs = &g_wifi_default_mesh_crypto_funcs
//		};
//		//Calling node initialization function
//		Mesh::Node::init(cfg, other_cfg);
//	}
//
///**
// * There is to possible configuration of Node/border router mesh.
// *
// * Meth::Border_Router<true>
// *
// * The configuration to which server messages will be forward is at the
// * node (if the border router is configured to connect to this server
// * is another thing).
// *
// *  Meth::Border_Router<false>
// *
// *  Nodes will send messages to the border router, and the border router
// *  will forward it to the server (the info is just at the BR).
// */
//#if USE_TODS_TASK == 1
//	mesh_addr_t to;
//	to.mip.ip4.addr = inet_addr(HOST_IP_ADDR);
//	to.mip.port = htons(HOST_PORT);
//	Mesh::Node::set_to_send(&to);
//	Mesh::Border_Router<true>(TCP_Client(HOST_IP_ADDR, HOST_PORT)).init();
//#else
//	Mesh::Border_Router<false>(TCP_Client(HOST_IP_ADDR, HOST_PORT)).init();
//#endif
//	/**
//	 * Registering callback function to print info when connect to parent
//	 */
//	Mesh::Node::register_event(
//    		MESH_EVENT_PARENT_CONNECTED,
//			[](void *arg, esp_event_base_t event_base,
//            	int32_t event_id, void *event_data){
//
//    	mesh_addr_t mac, parent, id;
//    	Mesh::Node::mac(&mac);
//    	Mesh::Node::parent(&parent);
//    	Mesh::Node::id(&id);
//
//        ESP_LOGI(TAG, "%s[%d]:[" MACSTR "]([" MACSTR "]->[" MACSTR "])",
//														Mesh::Node::topology() == MESH_TOPO_TREE
//														? "tree" : "chain",
//														Mesh::Node::layer(),
//														MAC2STR(id.addr),
//														MAC2STR(mac.addr),
//														MAC2STR(parent.addr));
//        ESP_LOGI(TAG, "is root: %s, is_ps: %s", Mesh::Node::is_root() ?
//        											"true" : "false",
//        										Mesh::Node::is_power_save() ?
//        											"true" : "false");
//    }, NULL);
//
//	/**
//	 * Hang there
//	 */
//	while(true) vTaskDelay(portMAX_DELAY);
//}
