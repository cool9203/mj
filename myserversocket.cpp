#include "myserversocket.h"
#include "clientsocket.h"

myserversocket::myserversocket(int socket, int port, int backlog, char *ip) {
	socketfd = socket;
	this->port = port;
	this->backlog = backlog;
	for (int i = 0; i < (int)(strlen(ip)); i++) {
		this->ip[i] = ip[i];
	}
}


myserversocket::myserversocket(int socket, int port, int backlog, std::string ip) {
	socketfd = socket;
	this->port = port;
	this->backlog = backlog;
	for (int i = 0; i < (int)(ip.size()); i++) {
		this->ip[i] = ip.at(i);
	}
}


myserversocket::~myserversocket() {
	close();
}


int myserversocket::listen() {
	socklink *sockaddr = new socklink(ip, port);
	sockaddr_in addr = sockaddr->get_struct();

	if (socketfd == -1) {
		return -1;
	}

	if (::bind(socketfd, (SOCKADDR*)&addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {  //接上SOCKET
		close();
		return -2;
	}

	std::cout << "Socket建立成功" << std::endl;

	if (::listen(socketfd, backlog) == SOCKET_ERROR) {
		close();
		return -3;
	}

	return 1;
}


clientsocket* myserversocket::accept() {
	sockaddr_in clientaddr;
	int size = sizeof(SOCKADDR);
	int clientfd = ::accept(socketfd, (struct sockaddr*)&clientaddr, &size);

	return new clientsocket(clientfd, clientaddr);
}