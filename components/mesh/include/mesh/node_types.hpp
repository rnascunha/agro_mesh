#ifndef MESH_NODE_TYPES_HPP__
#define MESH_NODE_TYPES_HPP__

#include <stdint.h>
#include "types.hpp"

namespace Mesh{

using Node_Version = uint8_t;
static constexpr const Node_Version node_version = 0;

enum class Message_Type : uint8_t{
	DATA = 0,
	COMMAND
};

enum class Command_Type : uint8_t{
	ROUTE_TABLE = 0,
	CONFIG,
	STATUS,
	FULL_CONFIG,
	REBOOT,
	INFO
};

struct __attribute__((packed)) Node_Header{
	Node_Version	version;
	Message_Type	type;
	uint16_t		size;
};

struct __attribute__((packed)) Message_Command{
	Command_Type	type;
};

struct __attribute__((packed)) Response_Command{
	Node_Header		header;
	Command_Type	type;
};

struct __attribute__((packed)) Response_Route_Table{
	Node_Header		header;
	Command_Type	command;
	uint8_t 		layer;
	mesh_addr_t		parent;
};

struct __attribute__((packed)) Response_Config{
	Node_Header		header;
	Command_Type	command;
	mesh_addr_t		mac_ap;
	mesh_addr_t		id;
	uint8_t			is_root;
	uint8_t			channel_config:4;
	uint8_t			channel_conn:4;
};

struct __attribute__((packed)) Response_Status{
	Node_Header		header;
	Command_Type	command;
	int8_t			rssi;
};

struct __attribute__((packed)) Response_Full_Config{
	Node_Header		header;
	Command_Type	command;
	mesh_addr_t		mac_ap;
	mesh_addr_t		id;
	uint8_t			is_root;
	uint8_t			channel_config:4;
	uint8_t			channel_conn:4;
	int8_t			rssi;
	uint8_t 		layer;
	mesh_addr_t		parent;
};

}//Mesh

#endif /* MESH_NODE_TYPES_HPP__ */
