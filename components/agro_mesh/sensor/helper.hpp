#ifndef AGRO_MESH_SENSOR_HELPER__
#define AGRO_MESH_SENSOR_HELPER__

#include <cstddef>
#include "sensor_type.hpp"
#include "../net/coap_engine.hpp"


/**
 *
 * @param message type: CoAP::Message::type::confirmable / CoAP::Message::type::nonconfirmable
 * @param buffer with payload to send
 * @param buffer size
 * @param report error
 * @return packet send size
 */
std::size_t send_sensors_data(CoAP::Message::type,
								const void*,
								std::size_t,
								CoAP::Error&) noexcept;
/**
 *
 * @param message type: CoAP::Message::type::confirmable / CoAP::Message::type::nonconfirmable
 * @param Sensor_Builder factory
 * @param report error
 * @return packet send size
 */
std::size_t send_sensors_data(CoAP::Message::type,
								Sensor_Builder const&,
								CoAP::Error&) noexcept;

/**
 *
 * @param resource node to add to coap engine root
 */
bool add_resource(engine::resource_node&) noexcept;

#endif /* AGRO_MESH_SENSOR_HELPER__ */
