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
