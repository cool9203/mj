#include "clientsocket.h"

clientsocket::clientsocket(int socket, sockaddr_in addr) {
	socketfd = socket;
	address = addr;

	sockaddr = new socklink(addr);
}


clientsocket::clientsocket(int fd, char *ip, int port) {
	socketfd = fd;
	this->port = port;
	for (int i = 0; i < (int)(strlen(ip)); i++) {
		this->ip[i] = ip[i];
	}
}


clientsocket::clientsocket(int fd, std::string ip, int port) {
	socketfd = fd;
	this->port = port;
	for (int i = 0; i < (int)(ip.size()); i++) {
		this->ip[i] = ip.at(i);
	}
}


void clientsocket::initial(int socket, sockaddr_in addr) {
	clientsocket(socket, addr);
}


void clientsocket::initial(int fd, char *ip, int port) {
	clientsocket(fd, ip, port);
}


void clientsocket::initial(int fd, std::string ip, int port) {
	clientsocket(fd, ip, port);
}


int clientsocket::connect() {
	socklink *sockaddr = new socklink(ip, port);
	sockaddr_in addr = sockaddr->get_struct();

	if (socketfd == -1) {
		return 899;
	}

	return ::connect(socketfd, (SOCKADDR*)&addr, sizeof(SOCKADDR));
}


socklink* clientsocket::get_socketaddress() {
	return sockaddr;
}