#include "mesh/border_router.hpp"
#include "mesh/node.hpp"

#include <type_traits>

#define MESH_BR_INTERNAL_TASK_NAME		"mesh_br_internal"
#define MESH_BR_INTERNAL_TASK_SIZE		3072
#define MESH_BR_INTERNAL_TASK_PRIORITY	5

namespace Mesh{

#define RX_SIZE         1024
static uint8_t rx_buf[RX_SIZE] = { 0, };
void internal_net_task(void* arg);
void internal_net_task_null(void* arg);

extern void node_fromds_message(mesh_addr_t* from, mesh_data_t* data, void* arg);

int br_fromds_message(mesh_addr_t* from, uint8_t* rx_buf, mesh_data_t* data, void* arg);
int br_tods_message(mesh_addr_t* from, uint8_t* rx_buf, mesh_data_t* data, void* arg);

template<>
void Border_Router<true>::start(){
	if(Node::is_root())
	{
		xTaskCreatePinnedToCore(internal_net_task,
				MESH_BR_INTERNAL_TASK_NAME, MESH_BR_INTERNAL_TASK_SIZE,
				this, MESH_BR_INTERNAL_TASK_PRIORITY, NULL, tskNO_AFFINITY);
	}
}

template<>
void Border_Router<false>::start()
{
	Node::stop();
	Node::tods_cb_arg(this);
	if(Node::is_root())
	{
		xTaskCreatePinnedToCore(internal_net_task_null,
						MESH_BR_INTERNAL_TASK_NAME, MESH_BR_INTERNAL_TASK_SIZE,
						this, MESH_BR_INTERNAL_TASK_PRIORITY, NULL, tskNO_AFFINITY);
	}
}

void __attribute__((weak))
internal_net_task_null(void* arg)
{
	ESP_LOGD(TAG, "BR internal_net_task_null init");

    esp_err_t err;
    mesh_addr_t from;
    mesh_data_t data;
    int flag = 0;
    data.data = rx_buf + sizeof(BR_Response_Header);

    while(Node::is_root())
    {
        data.size = RX_SIZE;

        err = esp_mesh_recv(&from, &data, portMAX_DELAY, &flag, NULL, 0);
        if (err == ESP_OK && data.size)
        {
        	if(flag == MESH_DATA_FROMDS)
        		br_fromds_message(&from, rx_buf, &data, Node::fromds_cb_arg());
			else
				br_tods_message(&from, rx_buf, &data, arg);
        } else
        	ESP_LOGE(TAG, "[size:%u] Error internal receiving: [%d/%s]",
        			data.size, err, esp_err_to_name(err));
    }
	ESP_LOGD(TAG, "BR internal_net_task_null fin");
    vTaskDelete(NULL);
}

template<typename Border_Router>
static int
send_to_ds(mesh_addr_t* from, uint8_t* header_arr, mesh_data_t* data, Border_Router* br)
{
	BR_Response_Header* header = reinterpret_cast<BR_Response_Header*>(header_arr);

	header->m_version = br_version;
	memcpy(&header->addr, from, sizeof(mesh_addr_t));
	header->size = data->size;

	int err = br->write_socket(rx_buf, data->size + sizeof(BR_Response_Header));
	if(err <= 0)
	{
		ESP_LOGE(TAG, "Error sending %d", err);
		br->reconnect();
	}

	return err;
}

void __attribute__((weak))
internal_net_task(void* arg)
{
	ESP_LOGD(TAG, "BR internal_net_task init");
	Mesh::Border_Router<true>* router = reinterpret_cast<Mesh::Border_Router<true>*>(arg);

	mesh_addr_t from, to;
	int flag = 0;

	while(Node::is_root())
	{
		mesh_data_t data;
		data.data = rx_buf + sizeof(BR_Response_Header);
		data.size = RX_SIZE;

		//Receiving from mesh net to external net
		esp_err_t err = esp_mesh_recv_toDS(&from,
											&to,
											&data,
											portMAX_DELAY,
											&flag,
											NULL,
											0);

		if(err == ESP_OK){
			ESP_LOGD(TAG, "Recv[" MACSTR " -> %s:%u][f:%d][%d]", MAC2STR(from.addr),
													ip4addr_ntoa(&to.mip.ip4),
													htons(to.mip.port),
													flag,
													data.size);

			send_to_ds(&from, rx_buf, &data, router);
		} else
			ESP_LOGE(TAG, "Error received socket data: %X (%s)", err, esp_err_to_name(err));
	}
	ESP_LOGD(TAG, "BR internal_net_task fin");
    vTaskDelete(NULL);
}

int __attribute__((weak))
br_tods_message(mesh_addr_t* from, uint8_t* header_arr, mesh_data_t* data, void* arg)
{
	Mesh::Border_Router<false>* router = reinterpret_cast<Mesh::Border_Router<false>*>(arg);

	return send_to_ds(from, header_arr, data, router);
}

int __attribute__((weak))
br_fromds_message(mesh_addr_t* from, uint8_t* header_arr, mesh_data_t* data, void* arg)
{
	node_fromds_message(from, data, arg);

	return 1;
}


}//Mesh
