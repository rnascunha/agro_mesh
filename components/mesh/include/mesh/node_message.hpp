#ifndef MESH_NODE_MESH_HPP__
#define MESH_NODE_MESH_HPP__

#include "esp_mesh.h"

namespace Mesh{

void
node_fromds_message(mesh_addr_t* from, mesh_data_t* data, void* arg);

void
node_tods_message(mesh_addr_t* from, mesh_data_t* data, void* arg);

void send_route_table();
void send_config();
void send_status();
void send_full_config();
void send_info(const char*);

}

#endif /* MESH_NODE_MESH_HPP__ */
