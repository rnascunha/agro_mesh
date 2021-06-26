#include "esp_log.h"
#include "../messages/make.hpp"
#include "../coap_engine.hpp"

extern const char* RESOURCE_TAG;

static void get_rssi_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get net rssi handler");

	/**
	 * Making the payload
	 */
	char buffer[20];

	wifi_ap_record_t ap;
	esp_wifi_sta_get_ap_info(&ap);
	snprintf(buffer, 20, "%d", ap.rssi);

	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Sending response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

static void put_waive_root_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put net waive root handler");

	if(!esp_mesh_is_root())
	{
		response
			.code(CoAP::Message::code::precondition_failed)
			.payload("not root")
			.serialize();
	}
	esp_mesh_waive_root(nullptr, MESH_VOTE_REASON_ROOT_INITIATED);

	/**
	 * Sending response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::success)
			.serialize();
}

static void get_parent_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get net parent handler");

	mesh_addr_t daddy{};
	esp_mesh_get_parent_bssid(&daddy);

	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	char buffer[20];
	snprintf(buffer, 20, MACSTR, MAC2STR(daddy.addr));

	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer)
		.serialize();
}

static void get_net_route_handler(engine::message const&, engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get net route handler");

	std::uint8_t buffer[engine::packet_size];
	CoAP::Error ec;
	std::size_t size = make_route(buffer, static_cast<std::size_t>(engine::packet_size), ec);

	if(ec)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload(ec.message())
			.serialize();
		return;
	}

	CoAP::Message::content_format format = CoAP::Message::content_format::application_octet_stream;
	CoAP::Message::Option::node content{format};

	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer, size)
		.serialize();
}

static void get_net_config_handler(engine::message const&, engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get net route handler");

	config cfg;

	CoAP::Message::content_format format = CoAP::Message::content_format::application_octet_stream;
	CoAP::Message::Option::node content{format};

	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(make_config(cfg), sizeof(config))
		.serialize();
}

static void get_net_full_config_handler(engine::message const&, engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get net route handler");

	std::uint8_t buffer[engine::packet_size];
	CoAP::Error ec;
	std::size_t size = make_full_config(buffer, static_cast<std::size_t>(engine::packet_size), ec);

	if(ec)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload(ec.message())
			.serialize();
		return;
	}

	CoAP::Message::content_format format = CoAP::Message::content_format::application_octet_stream;
	CoAP::Message::Option::node content{format};

	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer, size)
		.serialize();
}

engine::resource_node res_net_rssi{"rssi",
								get_rssi_handler};
engine::resource_node res_net_waive_root{"waive",
								static_cast<engine::resource::callback_t*>(nullptr),
								nullptr,
								put_waive_root_handler};
engine::resource_node res_net_parent{"parent",
								get_parent_handler};
engine::resource_node res_net_config{"config",
								get_net_config_handler};
engine::resource_node res_net_full_config{"full_config",
								get_net_full_config_handler};
engine::resource_node res_net_route{"route",
								get_net_route_handler};
