#include "helper.hpp"
#include "coap_engine.hpp"

extern engine coap_engine;

namespace Agro{

size_t coap_send(CoAP::Message::type mtype, CoAP::Message::code mcode,
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

}//Agro
