#pragma once
#include "mysocket.h"

class clientsocket : public mysocket{
protected:
	sockaddr_in address;
	socklink* sockaddr;

public:
	clientsocket(int, sockaddr_in);
	clientsocket(int, char*, int);
	clientsocket(int, std::string, int);

	int connect();
	socklink* get_socketaddress();
};