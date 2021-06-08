#include "mesh/node.hpp"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_mesh_internal.h"

#include "lwip/err.h"
#include "lwip/sockets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mesh/types.hpp"
#include "mesh/border_router.hpp"

#include <stdio.h>

#include <utility>      // std::move

#define TAG		"MESH_NODE"

namespace Mesh{

bool node_started = false;
static bool ds_state_connected = false;

static mesh_addr_t* to = NULL;

extern void node_receive_task(void *arg);
extern void node_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

void* Node::tods_arg = NULL;
void* Node::fromds_arg = NULL;

void Node::init(mesh_cfg_t& cfg, other_mesh_cfg_t& other_cfg)
{
	/*  mesh initialization */
	esp_mesh_init();
	register_event(ESP_EVENT_ANY_ID, &node_event_handler, NULL);
	/*  set mesh topology */
	topology(other_cfg.topology);
	/*  set mesh max layer according to the topology */
	esp_mesh_set_max_layer(other_cfg.max_layer);
	esp_mesh_set_vote_percentage(other_cfg.vote_percent);
	esp_mesh_set_xon_qsize(other_cfg.xon_queue);

	esp_mesh_set_ap_authmode(other_cfg.wifi_auth);
	/* Disable mesh PS function */
	power_save(other_cfg.enable_ps);
	esp_mesh_set_ap_assoc_expire(other_cfg.expire_sec);

	allow_root_conflicts(other_cfg.allow_root_conflicts);

	esp_mesh_set_config(&cfg);
	/* mesh start */
	esp_mesh_start();
}

void Node::deinit()
{
	unregister_event(ESP_EVENT_ANY_ID, node_event_handler);
}

void Node::start()
{
	if (!node_started) {
		node_started = true;
		xTaskCreatePinnedToCore(node_receive_task, "MPRX", 3072, NULL, 5, NULL, tskNO_AFFINITY);
	}
}

void Node::stop()
{
	node_started = false;
}

void Node::post_toDS_state(bool reachable)
{
	esp_mesh_post_toDS_state(reachable);
}

esp_err_t Node::send_to(mesh_data_t* data)
{
	return esp_mesh_send(NULL, data, 0, NULL, 0);
}

esp_err_t Node::send_to(const char* ip, uint16_t port, mesh_data_t* data)
{
	return Node::send_to(inet_addr(ip), port, data);
}

esp_err_t Node::send_to(uint32_t ip, uint16_t port, mesh_data_t* data)
{
	mesh_addr_t to;
	to.mip.ip4.addr = ip;
	to.mip.port = htons(port);

	return  Node::send_to(&to, data, MESH_DATA_TODS);
}

esp_err_t Node::send_to(mesh_addr_t* to, mesh_data_t* data, int flag /*= MESH_DATA_TODS */)
{
	return esp_mesh_send(to, data, flag, NULL, 0);
}

void Node::set_to_send(mesh_addr_t* to_)
{
	to = to_;
}

esp_err_t Node::to_send(mesh_data_t* data, int flag /* = MESH_DATA_TODS */)
{
	return Node::send_to(to, data, flag);
}

bool Node::is_root()
{
	return esp_mesh_is_root();
}

void Node::fix_root(bool en)
{
	esp_mesh_fix_root(en);
}

bool Node::is_fixed_root()
{
	return esp_mesh_is_root_fixed();
}

bool Node::allow_root_conflicts()
{
	return esp_mesh_is_root_conflicts_allowed();
}

void Node::allow_root_conflicts(bool allow)
{
	esp_mesh_allow_root_conflicts(allow);
}

void Node::power_save(bool en)
{
	en ? esp_mesh_enable_ps() : esp_mesh_disable_ps();
}

bool Node::is_power_save(){
	return esp_mesh_is_ps_enabled();
}

esp_mesh_topology_t Node::topology()
{
	return esp_mesh_get_topology();
}

void Node::topology(esp_mesh_topology_t topo)
{
	esp_mesh_set_topology(topo);
}

void Node::mac(mesh_addr_t* mac)
{
	esp_efuse_mac_get_default(mac->addr);
}

void Node::mac_ap(mesh_addr_t* mac_ap)
{
	esp_read_mac(mac_ap->addr, ESP_MAC_WIFI_SOFTAP);
}

void Node::parent(mesh_addr_t* bssid)
{
	esp_mesh_get_parent_bssid(bssid);
}

void Node::id(mesh_addr_t* id)
{
	esp_mesh_get_id(id);
}

int Node::layer()
{
	return esp_mesh_get_layer();
}

void Node::ds_state(bool connected)
{
	ds_state_connected = connected;
}

bool Node::ds_state()
{
	return ds_state_connected;
}

void Node::register_event(int32_t event_id,
        	esp_event_handler_t event_handler,
			void *event_handler_arg)
{
	esp_event_handler_register(MESH_EVENT, event_id, event_handler, event_handler_arg);
}

void Node::unregister_event(int32_t event_id,
						esp_event_handler_t event_handler)
{
	esp_event_handler_unregister(MESH_EVENT, event_id, event_handler);
}

void Node::tods_cb_arg(void* arg)
{
	tods_arg = arg;
}
void Node::fromds_cb_arg(void* arg)
{
	fromds_arg = arg;
}

void* Node::tods_cb_arg(void)
{
	return tods_arg;
}
void* Node::fromds_cb_arg(void)
{
	return fromds_arg;
}

}//Mesh
