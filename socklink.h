#pragma once
#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>

class socklink {
protected:
	char ip[20] = "";
	int port;
	std::string sip;

public:
	socklink(struct sockaddr_in);
	socklink(char*, int);
	socklink(std::string, int);

	sockaddr_in get_struct();

	int get_port();

	char* get_ip();

	std::string get_sip();

};
