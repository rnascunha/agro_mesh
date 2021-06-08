#ifndef MESH_TYPES_HPP__
#define MESH_TYPES_HPP__

#include <stdint.h>
#include "esp_mesh.h"

namespace Mesh{

//#define MESH_ADDR_SIZE		6
//typedef uint8_t mesh_addr[MESH_ADDR_SIZE];

struct other_mesh_cfg_t{
	esp_mesh_topology_t topology;
	float				vote_percent;
	int					xon_queue;
	int					expire_sec;
	int					max_layer;
	bool				enable_ps;
	wifi_auth_mode_t	wifi_auth;
	bool				allow_root_conflicts;
};

}//Mesh

#endif /* MESH_TYPES_HPP__ */
