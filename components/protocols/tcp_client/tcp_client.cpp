#include "tcp_client.hpp"
#include <cstring>

#include "lwip/err.h"

TCP_Client::TCP_Client(const char* ip, uint16_t port){
	dest_addr.sin_addr.s_addr = inet_addr(ip);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
}

TCP_Client::TCP_Client(const struct sockaddr_in& dest){
	memcpy(&dest_addr, &dest, sizeof(struct sockaddr_in));
	dest_addr.sin_family = AF_INET;
}

TCP_Client::TCP_Client(TCP_Client&& socket){
	memcpy(&dest_addr, &socket.dest_addr, sizeof(struct sockaddr_in));
	dest_addr.sin_family = AF_INET;
	this->socket = socket.socket;

	socket.socket = -1;
}

TCP_Client::err TCP_Client::open(){
	this->close();
	socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(socket < 0)	return socket_open_err;

	if (::connect(socket, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr))){
		this->close();
		return socket_connect_err;
	}

	return success;
}

void TCP_Client::close(){
	if(socket > 0){
		::shutdown(socket, 0);
		::close(socket);
	}
	socket = -1;
}

int TCP_Client::option(int optname, const void *optval, socklen_t optlen)
{
	return setsockopt(socket,
				SOL_SOCKET,
				optname,
				optval,
				optlen);
}

void TCP_Client::keep_alive(bool enable)
{
	int int_enable = enable ? 1 : 0;
	option(SO_KEEPALIVE, &int_enable, sizeof(int));
}

void TCP_Client::keep_alive(uint32_t idle, uint32_t count, uint32_t interval)
{
	keep_alive(true);

	::setsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));
	::setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(uint32_t));
	::setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(uint32_t));
}

int TCP_Client::read(void* data, std::size_t size, int flags /* = 0 */){
	return ::recv(socket, data, size, flags);
}
int TCP_Client::write(const void* data, std::size_t size, int flags /* = 0 */){
	return send(socket, data, size, 0);
}

