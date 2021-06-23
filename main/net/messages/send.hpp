#ifndef AGRO_MESH_NET_MESSAGES_SEND_HPP__
#define AGRO_MESH_NET_MESSAGES_SEND_HPP__

#include "coap-te.hpp"

template<typename Engine>
std::size_t send_route(Engine&, CoAP::Message::type, CoAP::Error ec) noexcept;
template<typename Engine>
std::size_t send_status(Engine&, CoAP::Message::type, CoAP::Error ec) noexcept;
template<typename Engine>
std::size_t send_config(Engine&, CoAP::Message::type, CoAP::Error ec) noexcept;
template<typename Engine>
std::size_t send_full_config(Engine&, CoAP::Message::type, CoAP::Error ec) noexcept;

#include "impl/send_impl.hpp"

#endif /* AGRO_MESH_NET_MESSAGES_SEND_HPP__ */
