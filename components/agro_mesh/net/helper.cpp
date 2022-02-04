#include "helper.hpp"
#include "coap_engine.hpp"
#include "messages/send.hpp"

extern engine coap_engine;

namespace Agro{

std::size_t coap_send(CoAP::Message::type mtype, CoAP::Message::code mcode,
				CoAP::Message::Option::node* options, size_t options_length,
				const void* data, size_t data_length,
				CoAP::Error& ec) noexcept
{
	if(mtype != CoAP::Message::type::confirmable
		&& mtype != CoAP::Message::type::nonconfirmable)
	{
		ec = CoAP::errc::type_invalid;
		return 0;
	}

	if(!CoAP::Message::is_request(mcode))
	{
		ec = CoAP::errc::code_invalid;
		return 0;
	}

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};

	engine::request req{ep};
	req.header(mtype, mcode)
		.payload(data, data_length);

	for(size_t i = 0; i < options_length; i++)
	{
		req.add_option(options[i]);
	}

	return coap_engine.send(req, ec);
}

std::size_t coap_send(engine::request& req, CoAP::Error& ec) noexcept
{
	return coap_engine.send(req, ec);
}

std::size_t send_config(CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	return ::send_config(coap_engine, mtype, ec);
}

std::size_t send_info(CoAP::Message::type mtype, const char* message, CoAP::Error& ec) noexcept
{
	return ::send_info(coap_engine, mtype, message, ec);
}

std::size_t send_report(CoAP::Message::type mtype,
						report_type rtype,
						const char* message,
						CoAP::Error& ec) noexcept
{
	return ::send_report(coap_engine, mtype, rtype, message, ec);
}

std::size_t send_app_list(CoAP::Message::type mtype,
						CoAP::Error& ec) noexcept
{
	return ::send_app_list(coap_engine, mtype, ec);
}

std::size_t send_job_info(CoAP::Message::type mtype, int index, CoAP::Error& ec) noexcept
{
	return ::send_job_info(coap_engine, mtype, index, ec);
}

void request_time(CoAP::Error& ec) noexcept
{
	::request_time(coap_engine, ec);
}

}//Agro
