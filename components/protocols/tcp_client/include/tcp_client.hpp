#ifndef ESP_TCP_CLIENT_HPP__
#define ESP_TCP_CLIENT_HPP__

#include <cstddef>
#include <cstdint>
#include "lwip/sockets.h"

class TCP_Client{
	public:
		TCP_Client(const char* ip, uint16_t port);
		TCP_Client(const struct sockaddr_in& dest);
		TCP_Client(TCP_Client&& socket);

		enum flags : int {
			Non_Blocking = MSG_DONTWAIT
		};

		enum Errno : int {
			Would_Block = EWOULDBLOCK
		};

		enum err : int {
			success = 1,
			socket_open_err = -1,
			socket_connect_err = -2
		};

		int option(int optname, const void *optval, socklen_t optlen);

		void keep_alive(bool enable);
		/*
		 * idle - seconds / count - number probes before timeout / interval - seconds
		 */
		void keep_alive(uint32_t idle, uint32_t count, uint32_t interval);

		err open();
		void close();

		int read(void* data, std::size_t size, int flags = 0);
		int write(const void* data, std::size_t size, int flags = 0);
	private:
		int socket = -1;
		struct sockaddr_in dest_addr;
};

#endif /* ESP_TCP_CLIENT_HPP__ */
