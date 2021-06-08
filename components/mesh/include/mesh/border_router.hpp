#ifndef MESH_BORDER_ROUTER_APP_HPP__
#define MESH_BORDER_ROUTER_APP_HPP__

#include "tcp_client.hpp"
#include "node.hpp"

namespace Mesh{

template<bool StartRecvTODS = true>
class Border_Router{
	public:
		static constexpr const bool start_recv_tods = StartRecvTODS;

		Border_Router(const char* ip, uint16_t port);
		Border_Router(TCP_Client&& socket);

		void init();
		void deinit();

		void start();
		void stop();

		void reconnect();

		void open();

		TCP_Client::err open_socket();
		void close_socket();

		int write_socket(const void* data, std::size_t size, int flags = 0);
		int read_socket(void* data, std::size_t size, int flags = 0);
	private:
		TCP_Client socket_;
};

}//Mesh

#include "../../br/border_router_impl.hpp"

#endif /* MESH_BORDER_ROUTER_APP_HPP__ */
