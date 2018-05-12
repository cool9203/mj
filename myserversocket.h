#pragma once
#include "mysocket.h"
#include "clientsocket.h"

class myserversocket : public mysocket {
protected:
	int backlog;

public:
	myserversocket(){}
	myserversocket(int socket, int port, int backlog, char *ip);
	myserversocket(int socket, int port, int backlog, std::string ip);
	~myserversocket();

	void initial(int socket, int port, int backlog, char *ip);
	void initial(int socket, int port, int backlog, std::string ip);
	int listen();

	clientsocket* accept();
};