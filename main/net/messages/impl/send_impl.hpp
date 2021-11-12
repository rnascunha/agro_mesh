#include "../send.hpp"
#include "../make.hpp"
#include <cstdint>
#include "../../../types/rtc_time.hpp"

static constexpr const char* route_path = "route";
static constexpr const char* config_path = "config";
static constexpr const char* full_config_path = "full_config";
static constexpr const char* board_path = "board";
static constexpr const char* info_path = "info";

extern RTC_Time device_clock;

template<bool RemoveSelf /* = true */, typename Engine>
std::size_t send_route(Engine& eng, CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	std::uint8_t buffer[Engine::packet_size];
	std::size_t size = make_route<RemoveSelf>(buffer, Engine::packet_size, ec);
	if(ec) return size;

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, route_path},
								content_format{ct};

	typename Engine::request req{ep};
	req.header(mtype, CoAP::Message::code::put)
		.add_option(uri_path)
		.add_option(content_format)
		.payload(buffer, size);

	return eng.send(req, ec);
}

template<typename Engine>
std::size_t send_config(Engine& eng, CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	config conf;
	make_config(conf);

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, config_path},
									content_format{ct};
	typename Engine::request req{ep};
	req.header(mtype, CoAP::Message::code::put)
		.add_option(uri_path)
		.add_option(content_format)
		.payload(&conf, sizeof(config));

	return eng.send(req, ec);
}

template<bool RemoveSelf /* = true */, typename Engine>
std::size_t send_full_config(Engine& eng, CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	std::uint8_t buffer[Engine::packet_size];
	std::size_t size = make_full_config<RemoveSelf>(buffer, Engine::packet_size, ec);
	if(ec) return size;

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, full_config_path},
								content_format{ct};

	typename Engine::request req{ep};
	req.header(mtype, CoAP::Message::code::put)
		.add_option(uri_path)
		.add_option(content_format)
		.payload(buffer, size);

	return eng.send(req, ec);
}

template<typename Engine>
std::size_t send_board_config(Engine& eng, CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	std::uint8_t buffer[Engine::packet_size];
	std::size_t size = make_board_config(buffer, Engine::packet_size, ec);
	if(ec) return size;

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, board_path},
								content_format{ct};

	typename Engine::request req{ep};
	req.header(mtype, CoAP::Message::code::put)
		.add_option(uri_path)
		.add_option(content_format)
		.payload(buffer, size);

	return eng.send(req, ec);
}

template<typename Engine>
std::size_t send_sensors_data(Engine& eng, CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	uint8_t dataaa[32];
	size_t size = make_sensors_data(dataaa, 32);

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, "sensors"},
									content_format{ct};
	typename Engine::request req{ep};

	req.header(mtype, CoAP::Message::code::put)
		.add_option(uri_path)
		.add_option(content_format)
		.payload(dataaa, size);

	return eng.send(req, ec);
}

template<typename Engine>
std::size_t send_info(Engine& eng, CoAP::Message::type mtype, const char* message, CoAP::Error& ec) noexcept
{
	CoAP::Message::content_format ct{CoAP::Message::content_format::text_plain};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, info_path},
										content_format{ct};

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};

	typename Engine::request req{ep};
	req.header(mtype, CoAP::Message::code::put)
		.add_option(uri_path)
		.add_option(content_format)
		.payload(message);

	return eng.send(req, ec);
}

static void request_time_cb(void const*,
		CoAP::Message::message const* response,
		void*) noexcept
{
	if(response)
	{
		device_clock.set_time(*static_cast<std::uint32_t const*>(response->payload));
	}
}

template<typename Engine>
void request_time(Engine& eng, CoAP::Error& ec) noexcept
{
	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, "time"},
									content_format{ct};
	typename Engine::request req{ep};
	req.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
		.add_option(uri_path)
		.add_option(content_format)
		.callback(request_time_cb);

	eng.send(req, ec);
}
