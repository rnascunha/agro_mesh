#ifndef AGRO_MESH_NET_SEND_DATA_HPP__
#define AGRO_MESH_NET_SEND_DATA_HPP__

#include "coap-te.hpp"

size_t send_sensors_data(CoAP::Message::type, CoAP::Error&) noexcept;
size_t send_board_config(CoAP::Message::type, CoAP::Error&) noexcept;

#endif /* AGRO_MESH_NET_SEND_DATA_HPP__ */
