#ifndef AGRO_MESH_NET_MESSAGES_MAKE_IMPL_HPP__
#define AGRO_MESH_NET_MESSAGES_MAKE_IMPL_HPP__

#include "../make.hpp"
#include <cstring>
#include "esp_system.h"

template<bool RemoveSelf /* = true */>
std::size_t make_route(void* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept
{
	if(buffer_len < sizeof(route))
	{
		ec = CoAP::errc::insufficient_buffer;
		return 0;
	}

	route* msg = reinterpret_cast<route*>(buffer);

	msg->layer = htons(static_cast<uint16_t>(esp_mesh_get_layer()));
	esp_mesh_get_parent_bssid(&msg->parent);

	int route_table_size = 0;
	esp_mesh_get_routing_table((mesh_addr_t *)(static_cast<uint8_t*>(buffer) + sizeof(route)),
									   buffer_len - sizeof(route), &route_table_size);

	if constexpr(RemoveSelf)
	{
		std::uint8_t *b = static_cast<uint8_t*>(buffer) + sizeof(route);
		std::uint8_t mac[6];
		esp_read_mac(mac, ESP_MAC_WIFI_STA);

		for(int i = 0; i < route_table_size; i++)
		{
			if(std::memcmp(b, mac, 6) == 0)
			{
				std::memcpy(b, b + 6, (route_table_size - i - 1) * 6);
				route_table_size--;
				break;
			}
			b += 6;
		}
	}

	return sizeof(route) + route_table_size * sizeof(mesh_addr_t);
}

template<bool RemoveSelf /* = true */>
std::size_t make_full_config(void* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept
{
	if(buffer_len < (sizeof(config) + sizeof(status) + sizeof(route)))
	{
		ec = CoAP::errc::insufficient_buffer;
		return 0;
	}

	full_config* msg = reinterpret_cast<full_config*>(buffer);

	make_config(msg->fconfig);
	make_status(msg->fstatus);

	return make_route<RemoveSelf>(&msg->froute, buffer_len - sizeof(config) - sizeof(status), ec)
			+ sizeof(config) + sizeof(status);
}

#endif /* AGRO_MESH_NET_MESSAGES_MAKE_IMPL_HPP__ */
