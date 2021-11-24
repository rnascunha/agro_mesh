#include "agro/mesh.hpp"
#include "agro/helper.hpp"

extern engine::resource_node res_packet_sensors;
extern engine::resource_node res_packet_board;

extern engine::resource_node res_ac_load;
extern engine::resource_node res_ac_load1;
extern engine::resource_node res_ac_load2;
extern engine::resource_node res_ac_load3;

engine::resource_node res_packet{"packet"};

void init_resources() noexcept
{
	res_packet.add_child(res_packet_board, res_packet_sensors);
	res_ac_load.add_child(res_ac_load1, res_ac_load2, res_ac_load3);

	add_resource(res_packet);
	add_resource(res_ac_load);
}
