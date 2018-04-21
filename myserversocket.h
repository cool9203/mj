#pragma once
#include "mysocket.h"
#include "clientsocket.h"

class myserversocket : public mysocket {
protected:
	int backlog;

public:
	myserversocket(int, int, int, char*);
	myserversocket(int, int, int, std::string);
	~myserversocket();

	int listen();

	clientsocket* accept();


};