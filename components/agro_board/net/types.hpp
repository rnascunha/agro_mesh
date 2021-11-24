#ifndef AGRO_MESH_NET_TYPES_MAIN_HPP__
#define AGRO_MESH_NET_TYPES_MAIN_HPP__

#include <cstdint>

struct __attribute__((packed)) board_config{
	std::uint8_t	has_temp_sensor:1;
	std::uint8_t	has_rtc:1;
	std::uint8_t	:6;
};

#endif /* AGRO_MESH_NET_TYPES_MAIN_HPP__ */
