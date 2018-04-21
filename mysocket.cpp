#include <iostream>
#include <mutex>
#include <string>
#include <iostream>
#include "mysocket.h"
#pragma comment(lib, "Ws2_32.lib")

char mysocket::itoc(int data) {
	return static_cast<char>(data);
}


void mysocket::set_blocking() {
	mtx.lock();
}


void mysocket::set_unblocking() {
	mtx.unlock();
}


#pragma region read
int mysocket::read(std::string &buf, int len) {
	char data[4096];
	int r = read(data, len);
	int size = (static_cast<int>(data[0]) - 1) * 127 + static_cast<int>(data[1]);
	data[size + 1] = '}';

	for (int i = 0; i < size; i++) {
		buf.push_back(data[i+2]);
	}

	return r;
}


int mysocket::read(json &jsn, int len) {
	std::string str;
	int r = read(str, len);
	//std::cout << str;
	jsn = json::parse(str);
	return r;
}


int mysocket::read(json &jsn) {
	return read(jsn, 4096);
}


int mysocket::read(char *buf, int len) {
	this->set_blocking();
	int r = ::recv(socketfd, buf, len, 0);
	this->set_unblocking();
	return r;
}
#pragma endregion </spin>


#pragma region send
int mysocket::send(std::string data) {
	char buf[4096] = "";

	int size = data.size(); ////¥ý¨ú±odata.size()
	int mul = 1;
	if (size > 127) {
		size -= 127;
		mul++;
	}
	buf[0] = itoc(mul);
	buf[1] = itoc(size);

	for (int i = 0; i < static_cast<int>(data.size()); i++) {
		buf[i + 2] = data.at(i);
	}
	buf[static_cast<int>(data.size()) + 1] = '\0';

	return send(buf);
}


int mysocket::send(char *data) {
	return send(data, strlen(data), 0);
}


int mysocket::send(json &jsn) {
	std::string str = jsn.dump();
	//std::cout << str << "\n\n";
	return send(str);
}


int mysocket::send(char *data, int len, int flags) {
	this->set_blocking();
	int s = ::send(socketfd, data, len, flags);
	this->set_unblocking();
	return s;
}
#pragma endregion </spin>


int mysocket::getsocket() {
	return socketfd;
}


char* mysocket::get_ip() {
	return ip;
}


std::string mysocket::getsip() {
	std::string str;
	for (int i = 0; i < static_cast<int>(strlen(ip)); i++) {
		str += ip[i];
	}
	return str;
}


int mysocket::getport() {
	return port;
}


bool mysocket::status() {
	return socketfd != -1;
}


void mysocket::close() {
	if (socketfd == -1) {
		return;
	}

	::closesocket(socketfd);
}