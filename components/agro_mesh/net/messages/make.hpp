#ifndef AGRO_MESH_NET_MESSAGES_MAKE_HPP__
#define AGRO_MESH_NET_MESSAGES_MAKE_HPP__

#include "types.hpp"
#include <cstdlib>
#include "coap-te.hpp"

template<bool RemoveSelf = true>
std::size_t make_route(void* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;
config* make_config(config& cfg) noexcept;
template<bool RemoveSelf = true>
std::size_t make_full_config(void* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;

#include "impl/make_impl.hpp"

#endif /* AGRO_MESH_NET_MESSAGES_MAKE_HPP__ */
