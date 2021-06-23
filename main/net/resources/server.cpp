#include "esp_log.h"
#include "../coap_engine.hpp"
#include "../../helper/decompose.hpp"

#include <cstring>

extern const char* RESOURCE_TAG;

extern udp_conn::endpoint ep_server;

static void get_server_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get server handler");

	char buffer[50];
	snprintf(buffer, 20, "%s:%u",ep_server.address(buffer, 20), ep_server.port());

	/**
	 * Option
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

static void put_server_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put server handler");

	if(request.payload_len == 0 || request.payload_len > 50)
	{
		response
			.code(CoAP::Message::code::bad_request)
			.serialize();
		return;
	}

	char addr_str[50];
	std::memcpy(addr_str, request.payload, request.payload_len);
	addr_str[request.payload_len] = '\0';

	CoAP::URI::uri<in_addr> uri;
	if(!decompose_ipv4(uri, addr_str))
	{
		response
			.code(CoAP::Message::code::bad_request)
			.serialize();
		return;
	}

	CoAP::Error ec;
	udp_conn::endpoint ep{uri.host.s_addr, uri.port};
	char buffer[20];
	ESP_LOGI(RESOURCE_TAG, "server uri: %s:%u", ep.address(buffer), uri.port);

	response
		.code(CoAP::Message::code::changed)
		.serialize();
}

engine::resource_node res_net_server("server",
								get_server_handler,
								nullptr,
								put_server_handler);
