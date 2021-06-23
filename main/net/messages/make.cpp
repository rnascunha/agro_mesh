#include "make.hpp"
#include "esp_system.h"
#include <cstring>

std::size_t make_route(void* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
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

	return sizeof(route) + route_table_size * sizeof(mesh_addr_t);
}

config* make_config(config& cfg) noexcept
{
	mesh_cfg_t cfg_;
	wifi_ap_record_t ap;

	esp_mesh_get_config(&cfg_);
	esp_wifi_sta_get_ap_info(&ap);

	esp_read_mac(cfg.mac_ap.addr, ESP_MAC_WIFI_SOFTAP);
	std::memcpy(&cfg.net_id.addr, &cfg_.mesh_id.addr, 6);
	cfg.channel_config = cfg_.channel;
	cfg.channel_conn = ap.primary;

	return &cfg;
}

status* make_status(status& sts) noexcept
{
	wifi_ap_record_t ap;
	esp_wifi_sta_get_ap_info(&ap);

	sts.rssi = ap.rssi;

	return &sts;
}

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

	return make_route(&msg->froute, buffer_len - sizeof(config) - sizeof(status), ec)
			+ sizeof(config) + sizeof(status);
}
