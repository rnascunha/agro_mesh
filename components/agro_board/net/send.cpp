#include "send.hpp"
#include "make.hpp"

#include "agro/helper.hpp"

size_t send_sensors_data(CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	uint8_t data[24];
	size_t size = make_sensors_data(data, 24);

	return send_sensors_data(mtype, data, size, ec);
}

size_t send_board_config(CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	uint8_t data[engine::packet_size];
	size_t size = make_board_config(data, engine::packet_size);

	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, "board"};
	return Agro::coap_send(mtype, CoAP::Message::code::put, &uri_path, 1, data, size, ec);
}
