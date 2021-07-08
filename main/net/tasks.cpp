#include "coap-te/log.hpp"				//Log header

#include "coap_engine.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_mesh.h"

#include "messages/send.hpp"
#include "types.hpp"

using namespace CoAP::Log;

#define TAG		"TASK"

bool coap_engine_started = false;
#define RX_SIZE          (CONFIG_BUFFER_TRANSACTION)
/**
 * Receving buffer of proxy
 */
static uint8_t rx_buf[RX_SIZE];

extern xQueueHandle send_data_queue;

CoAP::Error ecp;
/**
 * Server endpoint where all the internal packets will be forward
 */
udp_conn::endpoint ep_server{CONFIG_SERVER_ADDR, CONFIG_SERVER_PORT, ecp};

extern engine coap_engine;

void coap_te_engine(void*) noexcept
{
	ESP_LOGI(TAG, "CoAP Engine run init.");

	CoAP::Error ec;
	while(coap_engine.run<50>(ec)){}

	coap_engine_started = false;
	Packet_Type t = Packet_Type::no_packet;
	xQueueSend(send_data_queue, &t, 0);
	vTaskDelete(NULL);
}

/**
 * This function will route the packets from the IP network to the
 * mesh network.
 *
 * It's only initialized if the node is the root of the mesh tree network.
 *
 * It will open a UDP connection and bind to the port configured.
 */
void coap_forward_proxy(void*) noexcept
{
	ESP_LOGD(TAG, "Proxy forwaring task init");

	/**
	 * Opening UDP socket
	 */
	udp_conn conn{};
	conn.open(ecp);
	if(ecp)
	{
		ESP_LOGE(TAG, "Error opening socket [%d/%s]", ecp.value(), ecp.message());
		/**
		 * As we could not open a socket to forward the packets, we are giving up
		 * from being root!
		 */
		esp_mesh_waive_root(nullptr, MESH_VOTE_REASON_ROOT_INITIATED);
		vTaskDelete(NULL);
		return;
	}

	/**
	 * Bind socket to the configured port
	 */
	udp_conn::endpoint bind_ep{CONFIG_ROOT_SERVER_PORT};
	conn.bind(bind_ep, ecp);
	if(ecp)
	{
		ESP_LOGE(TAG, "Error binding [%d/%s]", ecp.value(), ecp.message());
	}

	/**
	 * We are going to work as forward proxy as long as we are root
	 */
	while(esp_mesh_is_root())
	{
		/**
		 * `proxy_forward_udp_mesh` will deal with all complexity to forward
		 * packets between networks.
		 *
		 * The template parameter is the block reading time to each network, in milliseconds.
		 */
		CoAP::Error ec;
		CoAP::Port::ESP_Mesh::proxy_forward_udp_mesh<50>(
				conn,				///< UDP connection
				ep_server,			///< Endpoint of IP networ to forward
				coap_engine,		///< CoAP engine
				rx_buf,				///< Buffer to be used to receive packtes
				RX_SIZE,			///< Buffer size
				ec);				///< Error report
		if(ec)
		{
			ESP_LOGE(TAG, "Error proxy [%d/%s]", ec.value(), ec.message());
		}
	}
	ESP_LOGD(TAG, "Proxy forwaring task end");

	esp_mesh_post_toDS_state(false);
	conn.close();
	vTaskDelete(NULL);
}

/**
 * Send periodic data
 */
void coap_send_main(void*) noexcept
{
	while(coap_engine_started)
	{
		CoAP::Error ec;
		send_sensor_data(coap_engine, CoAP::Message::type::nonconfirmable, ec);
		if(ec)
		{
			ESP_LOGE(TAG, "ERROR sending time request [%d/%s]...", ec.value(), ec.message());
		}
		else
		{
			ESP_LOGI(TAG, "Sending time request...");
		}
		vTaskDelay((5 * 60 * 1000) / portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}


/**
 * Async send data
 */
void send_async_data(void*) noexcept
{
	Packet_Type type;
	while(coap_engine_started)
	{
		if(xQueueReceive(send_data_queue, &type, portMAX_DELAY))
		{
			switch(type)
			{
				case Packet_Type::sensor:
				{
					CoAP::Error ec;
					send_sensor_data(coap_engine, CoAP::Message::type::nonconfirmable, ec);
				}
				break;
				default:
					break;
			}
		}
	}
	vTaskDelete(NULL);
}
