#include "esp_log.h"
#include "../make.hpp"
#include "agro/mesh.hpp"

extern const char* RESOURCE_TAG;

static void get_packet_sensors_handler(engine::message const&, engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get packet sensors handler");

	uint8_t data[24];
	size_t size = make_sensors_data(data, 24);

	CoAP::Message::content_format format = CoAP::Message::content_format::application_octet_stream;
	CoAP::Message::Option::node content{format};

	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(data, size)
		.serialize();
}

static void get_packet_board_handler(engine::message const&, engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get packet board handler");

	std::uint8_t buffer[engine::packet_size];
	std::size_t size = make_board_config(buffer, static_cast<std::size_t>(engine::packet_size));

	if(!size)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("Error creating packet")
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

engine::resource_node res_packet_sensors{"sensors",
							get_packet_sensors_handler};
engine::resource_node res_packet_board{"board",
							get_packet_board_handler};
