#ifndef AGRO_MESH_NET_HELPER_IMPL_HPP__
#define AGRO_MESH_NET_HELPER_IMPL_HPP__

#include "../helper.hpp"
#include "../coap_engine.hpp"
#include "../messages/send.hpp"

extern engine coap_engine;

namespace Agro{

template<bool RemoveSelf /* = true */>
std::size_t send_route(CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	return ::send_route<RemoveSelf>(coap_engine, mtype, ec);
}

template<bool RemoveSelf /* = true */>
std::size_t send_full_config(CoAP::Message::type mtype, CoAP::Error& ec) noexcept
{
	return ::send_full_config<RemoveSelf>(coap_engine, mtype, ec);
}

}//Agro

#endif /* AGRO_MESH_NET_HELPER_IMPL_HPP__ */
