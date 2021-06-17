#ifndef AGRO_MESH_COAP_ENGINE_HPP__
#define AGRO_MESH_COAP_ENGINE_HPP__

#include "coap-te.hpp"			//Convenient header

#if CONFIG_ENDPOINT_TYPE == 0
using endpoint = CoAP::Port::POSIX::endpoint_ipv4;
#else
using endpoint = CoAP::Port::POSIX::endpoint_ipv6;
#endif

/**
 * UDP socket type that will forward packtes from networks
 */
using udp_conn = CoAP::Port::POSIX::udp<endpoint>;

/**
 * The mesh socket, implemented to support CoAP-te library.
 *
 * The template parameter defines the type of service used
 * <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_mesh.html#_CPPv410mesh_tos_t>
 * By default it's MESH_TOS_P2P
 */
using mesh_socket = typename CoAP::Port::ESP_Mesh::socket<>;

/**
 * Engine definition. Check 'raw_engine' example for a full
 * description os the options.
 *
 * Here we are using server profile, and defining a resource callback
 * function. Server profile allow to add resources. We are also disabling
 * the default callback.
 */
using engine = CoAP::Transmission::engine<
		mesh_socket,
		CoAP::Message::message_id,
		CoAP::Transmission::transaction_list<
			CoAP::Transmission::transaction<
				CONFIG_BUFFER_TRANSACTION,
				CoAP::Transmission::transaction_cb,
				mesh_socket::endpoint>,
			CONFIG_TRANSACTION_NUM>,
		CoAP::disable,		//default callback disabled
		CoAP::Resource::resource<
			CoAP::Resource::callback<mesh_socket::endpoint>,
			true
		>
	>;

#endif /* AGRO_MESH_COAP_ENGINE_HPP__ */
