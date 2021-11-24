#ifndef AGRO_MESH_NET_MAKE_PACKET_DATA_HPP__
#define AGRO_MESH_NET_MAKE_PACKET_DATA_HPP__

#include <cstddef>
#include <cstdint>

std::size_t make_board_config(void*, std::size_t) noexcept;;
std::size_t make_sensors_data(std::uint8_t*, std::size_t) noexcept;

#endif /* AGRO_MESH_NET_MAKE_PACKET_DATA_HPP__ */
