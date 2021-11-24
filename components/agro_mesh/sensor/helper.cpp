#include "helper.hpp"
#include "../net/helper.hpp"

extern engine coap_engine;

std::size_t send_sensors_data(CoAP::Message::type mtype,
								const void* data,
								std::size_t size,
								CoAP::Error& ec) noexcept
{
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, "sensors"};
	return Agro::coap_send(mtype, CoAP::Message::code::put, &uri_path, 1, data, size, ec);

//	if(mtype != CoAP::Message::type::confirmable
//		&& mtype != CoAP::Message::type::nonconfirmable)
//	{
//		ec = CoAP::errc::type_invalid;
//		return 0;
//	}
//
//	//Making dummy endpoint
//	mesh_addr_t addr{};
//	addr.mip.port = htons(5683);
//	engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
//	//Making request
//	CoAP::Message::content_format ct{CoAP::Message::content_format::application_octet_stream};
//	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, "sensors"},
//								content_format{ct};
//
//	engine::request req{ep};
//	req.header(mtype, CoAP::Message::code::put)
//		.add_option(uri_path)
//		.add_option(content_format)
//		.payload(data, size);
//
//	return coap_engine.send(req, ec);
}

std::size_t send_sensors_data(CoAP::Message::type mtype,
								Sensor_Builder const& builder,
								CoAP::Error& ec) noexcept
{
	return send_sensors_data(mtype, builder.buffer(), builder.size(), ec);
}

bool add_resource(engine::resource_node& resource)
{
	return coap_engine.root_node().add_child(resource);
}
