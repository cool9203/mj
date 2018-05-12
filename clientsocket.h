#pragma once
#include "mysocket.h"

class clientsocket : public mysocket{
protected:
	sockaddr_in address;
	socklink* sockaddr;

public:
	clientsocket(){}
	clientsocket(int socket, sockaddr_in addr);
	clientsocket(int socket, char *ip, int port);
	clientsocket(int socket, std::string ip, int port);

	void initial(int socket, sockaddr_in addr);
	void initial(int socket, char *ip, int port);
	void initial(int socket, std::string ip, int port);

	int connect();
	socklink* get_socketaddress();
};