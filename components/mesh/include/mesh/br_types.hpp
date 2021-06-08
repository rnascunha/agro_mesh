#ifndef MESH_BORDER_ROUTER_TYPES_HPP__
#define MESH_BORDER_ROUTER_TYPES_HPP__

#include "types.hpp"
#include <stdint.h>

namespace Mesh{

enum class BR_Message_Type : uint8_t{
	DATA = 0,
	COMMAND_BR
};

enum class BR_Command_Type : uint8_t{
	ROUTE_TABLE = 0,
	CONFIG,
	STATUS,
	FULL_CONFIG,
	REBOOT,
	WAIVE_ROOT
};

using BR_Version = uint8_t;
static constexpr const BR_Version br_version = 0;

struct __attribute__((packed)) BR_Header{
	BR_Version 		m_version;
	BR_Message_Type	type;
	uint16_t		size;
};

struct __attribute__((packed)) BR_Response_Header{
	BR_Version 		m_version;
	mesh_addr_t		addr;
	uint16_t		size;
};

struct __attribute__((packed)) BR_Message_Data{
	mesh_addr_t		addr;
};

struct __attribute__((packed)) BR_Message_Command{
	BR_Command_Type	command;
};


}//Mesh

#endif /* MESH_BORDER_ROUTER_TYPES_HPP__ */
