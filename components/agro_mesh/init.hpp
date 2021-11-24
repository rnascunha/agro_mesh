#ifndef AGRO_MESH_INIT_HPP__
#define AGRO_MESH_INIT_HPP__

void init_mesh() noexcept;
void init_coap_resources() noexcept;

void coap_send_main(void*) noexcept;
bool ds_state() noexcept;

#endif /* AGRO_MESH_INIT_HPP__ */
