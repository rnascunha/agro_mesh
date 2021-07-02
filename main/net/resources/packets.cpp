#include "esp_log.h"
#include "../messages/make.hpp"
#include "../coap_engine.hpp"

extern const char* RESOURCE_TAG;

static void get_packet_sensor_handler(engine::message const&, engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get packet sensor handler");

	sensor_data data;

	CoAP::Message::content_format format = CoAP::Message::content_format::application_octet_stream;
	CoAP::Message::Option::node content{format};

	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(make_sensor_data(data), sizeof(config))
		.serialize();
}

static void get_packet_board_handler(engine::message const&, engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get packet board handler");

	std::uint8_t buffer[engine::packet_size];
	CoAP::Error ec;
	std::size_t size = make_board_config(buffer, static_cast<std::size_t>(engine::packet_size), ec);

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

engine::resource_node res_packet_sensor{"sensor",
							get_packet_sensor_handler};
engine::resource_node res_packet_board{"board",
							get_packet_board_handler};
