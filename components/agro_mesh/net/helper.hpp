#ifndef AGRO_MESH_NET_HELPER_HPP__
#define AGRO_MESH_NET_HELPER_HPP__

#include <stddef.h>
//#include "coap-te.hpp"
#include "coap_engine.hpp"
#include "messages/report.hpp"

namespace Agro{

std::size_t coap_send(CoAP::Message::type mtype, CoAP::Message::code mcode,
				CoAP::Message::Option::node* options, size_t options_length,
				const void* data, size_t data_length,
				CoAP::Error& ec) noexcept;

std::size_t coap_send(engine::request& req, CoAP::Error& ec) noexcept;

/**
 * \brief Check if mesh network is connected to external (IP) network
 *
 * \note It checks the 'ds' state flag
 * @return
 */
bool is_connected() noexcept;

template<bool RemoveSelf = true>
std::size_t send_route(CoAP::Message::type, CoAP::Error& ec) noexcept;
template<bool RemoveSelf = true>
std::size_t send_full_config(CoAP::Message::type, CoAP::Error& ec) noexcept;
std::size_t send_config(CoAP::Message::type, CoAP::Error& ec) noexcept;
std::size_t send_info(CoAP::Message::type, const char* message, CoAP::Error& ec) noexcept;
std::size_t send_report(CoAP::Message::type,
						report_type,
						const char* message,
						CoAP::Error& ec) noexcept;
std::size_t send_app_list(CoAP::Message::type,
						CoAP::Error& ec) noexcept;
std::size_t send_job_info(CoAP::Message::type, int index, CoAP::Error&) noexcept;
void request_time(CoAP::Error& ec) noexcept;

}//Agro

#include "impl/helper_impl.hpp"

#endif /* AGRO_MESH_NET_HELPER_HPP__ */
