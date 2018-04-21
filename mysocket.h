#pragma once
#include <WinSock2.h>
#include <mutex>
#include <string>
#include <iostream>
#include "socklink.h"
#include "mylib\single_include\nlohmann\json.hpp"
#pragma comment(lib, "Ws2_32.lib")
using json = nlohmann::json;

class mysocket {
protected:
	int socketfd;
	int port;
	char ip[20];
	std::mutex mtx;

	char itoc(int);

public:
	void set_blocking();
	void set_unblocking();

	int read(std::string&,int);
	int read(char*,int);
	int read(json&,int);
	int read(json&);

	int send(std::string);
	int send(char*);
	int send(char*, int, int);
	int send(json&);

	int getsocket();

	char* get_ip();

	std::string getsip();

	int getport();

	bool status();

	void close();

};