#ifndef AGRO_MESH_NET_HELPER_HPP__
#define AGRO_MESH_NET_HELPER_HPP__

#include <stddef.h>
#include "coap-te.hpp"

namespace Agro{

size_t coap_send(CoAP::Message::type mtype, CoAP::Message::code mcode,
				CoAP::Message::Option::node* options, size_t options_length,
				const void* data, size_t data_length,
				CoAP::Error& ec) noexcept;

}//Agro

#endif /* AGRO_MESH_NET_HELPER_HPP__ */
