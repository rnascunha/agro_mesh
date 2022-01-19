#ifndef AGRO_MESH_NET_MESSAGES_SEND_HPP__
#define AGRO_MESH_NET_MESSAGES_SEND_HPP__

#include "coap-te.hpp"
#include "report.hpp"

template<bool RemoveSelf = true, typename Engine>
std::size_t send_route(Engine&, CoAP::Message::type, CoAP::Error& ec) noexcept;
template<typename Engine>
std::size_t send_config(Engine&, CoAP::Message::type, CoAP::Error& ec) noexcept;
template<bool RemoveSelf = true, typename Engine>
std::size_t send_full_config(Engine&, CoAP::Message::type, CoAP::Error& ec) noexcept;
template<typename Engine>
std::size_t send_info(Engine&, CoAP::Message::type, const char* message, CoAP::Error& ec) noexcept;
template<typename Engine>
std::size_t send_report(Engine&,
						CoAP::Message::type,
						report_type,
						const char* message,
						CoAP::Error& ec) noexcept;
template<typename Engine>
std::size_t send_app_list(Engine&,
						CoAP::Message::type,
						CoAP::Error& ec) noexcept;
template<typename Engine>
void request_time(Engine& eng, CoAP::Error& ec) noexcept;

#include "impl/send_impl.hpp"

#endif /* AGRO_MESH_NET_MESSAGES_SEND_HPP__ */
