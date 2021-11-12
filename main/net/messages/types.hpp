#ifndef AGRO_MESH_NET_MESSAGES_TYPES_HPP__
#define AGRO_MESH_NET_MESSAGES_TYPES_HPP__

#include <cstdint>
#include "esp_mesh.h"

struct __attribute__((packed)) route{
	mesh_addr_t 	parent;
	std::uint16_t	layer;
};

struct __attribute__((packed)) config{
	mesh_addr_t		mac_ap;
	mesh_addr_t		net_id;
	uint8_t			channel_config:4;
	uint8_t			channel_conn:4;
};

struct __attribute__((packed)) full_config{
	config			fconfig;
	int8_t			rssi;
	route			froute;
};

struct __attribute__((packed)) board_config{
	std::uint8_t	has_temp_sensor:1;
	std::uint8_t	has_rtc:1;
	std::uint8_t	:6;
};

#endif /* AGRO_MESH_NET_MESSAGES_TYPES_HPP__ */
