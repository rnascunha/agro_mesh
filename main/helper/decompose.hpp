#ifndef AGRO_MESH_HELPER_DECOMPOSE_HPP__
#define AGRO_MESH_HELPER_DECOMPOSE_HPP__

#include "coap-te.hpp"
#include "lwip/sockets.h"

bool decompose_ipv4(CoAP::URI::uri<in_addr>& uri, char* uri_string) noexcept;

#endif /* AGRO_MESH_HELPER_DECOMPOSE_HPP__ */
