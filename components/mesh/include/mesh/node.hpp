#ifndef MESH_NODE_HPP__
#define MESH_NODE_HPP__

#include <stdint.h>
#include "types.hpp"

#include "esp_mesh.h"
#include "esp_err.h"

#include "esp_netif_types.h"

namespace Mesh{

class Node{
	public:
		static void init(mesh_cfg_t& cfg, other_mesh_cfg_t& other_cfg);
		static void deinit();

		static void start();
		static void stop();

		static bool is_root();
		static void fix_root(bool en);
		static bool is_fixed_root();

		static bool allow_root_conflicts();
		static void allow_root_conflicts(bool);

		static esp_mesh_topology_t topology();
		static void topology(esp_mesh_topology_t);

		static void power_save(bool en);
		static bool is_power_save();

		static void mac(mesh_addr_t*);
		static void mac_ap(mesh_addr_t*);
		static void parent(mesh_addr_t*);
		static void id(mesh_addr_t*);

		static int layer();

		static void post_toDS_state(bool);

		//to root
		static esp_err_t send_to(mesh_data_t*);
		//to external networt
		static esp_err_t send_to(const char* ip, uint16_t port, mesh_data_t*);
		static esp_err_t send_to(uint32_t ip, uint16_t port, mesh_data_t*);

		static esp_err_t send_to(mesh_addr_t*, mesh_data_t*, int flag = MESH_DATA_TODS);

		static void set_to_send(mesh_addr_t* to);
		static esp_err_t to_send(mesh_data_t*, int flag = MESH_DATA_TODS);

		static void ds_state(bool connected);
		static bool ds_state();

		static void tods_cb_arg(void*);
		static void fromds_cb_arg(void*);

		static void* tods_cb_arg(void);
		static void* fromds_cb_arg(void);

		static void register_event(int32_t event_id,
                	esp_event_handler_t event_handler,
					void *event_handler_arg);

		static void unregister_event(int32_t event_id,
								esp_event_handler_t event_handler);
	private:
		static void* tods_arg;
		static void* fromds_arg;
};

void wifi_default_init(esp_netif_t* netif_sta);

#define MESH_NODE_DEFAULT_OTHER_CONFIG {\
		.topology = MESH_TOPO_TREE, 	\
		.vote_percent = 1, 				\
		.xon_queue = 128,				\
		.expire_sec = 10,				\
		.max_layer = 6,					\
		.enable_ps = false,				\
		.wifi_auth = WIFI_AUTH_WPA2_PSK,\
		.allow_root_conflicts = false	\
	}

}//Mesh

#endif /* MESH_NODE_HPP__ */
