#include "../send.hpp"
#include "../make.hpp"
#include <cstdint>

static constexpr const char* route_path = "route";
static constexpr const char* status_path = "status";
static constexpr const char* config_path = "config";
static constexpr const char* full_config_path = "full_config";

template<typename Engine>
std::size_t send_route(Engine& eng, CoAP::Message::type mtype, CoAP::Error ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	std::uint8_t buffer[Engine::packet_size];
	std::size_t size = make_route(buffer, Engine::packet_size, ec);
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
std::size_t send_status(Engine& eng, CoAP::Message::type mtype, CoAP::Error ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	status sts;
	make_status(sts);

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	typename Engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, status_path},
									content_format{ct};
	typename Engine::request req{ep};
	req.header(mtype, CoAP::Message::code::put)
		.add_option(uri_path)
		.add_option(content_format)
		.payload(&sts, sizeof(status));

	return eng.send(req, ec);
}

template<typename Engine>
std::size_t send_config(Engine& eng, CoAP::Message::type mtype, CoAP::Error ec) noexcept
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

template<typename Engine>
std::size_t send_full_config(Engine& eng, CoAP::Message::type mtype, CoAP::Error ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	//Payload
	std::uint8_t buffer[Engine::packet_size];
	std::size_t size = make_full_config(buffer, Engine::packet_size, ec);
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
