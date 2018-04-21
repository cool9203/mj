#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <sstream>
#include <string>
#include "socklink.h"

socklink::socklink(struct sockaddr_in addr) {
	socklink::port = addr.sin_port;
	char ip[20];
	//取得(addr.sin_addr)的IPv4的文字ip
	InetNtop(PF_INET, &(addr.sin_addr), ip, strlen(ip));
	//轉型char[] to string
	for (int i = 0; i < (int)(strlen(ip)); i++) {
		this->ip[i] = ip[i];
		sip += ip[i];
	}
}


socklink::socklink(char *ip, int port) {
	for (int i = 0; i < (int)(strlen(ip)); i++) {
		this->ip[i] = ip[i];
	}

	this->port = port;
}

socklink::socklink(std::string ip,int port) {
	for (int i = 0; i < (int)(ip.size()); i++) {
		this->ip[i] = ip.at(i);
		sip += ip.at(i);
	}

	this->port = port;
}


sockaddr_in socklink::get_struct() {
	//先建一個address_in的接口
	sockaddr_in sock;
	//set
	memset(&sock, 0, sizeof(sock));
	int buf[20];
	//get (char)ip 的網路套接字
	InetPton(PF_INET, ip, buf);
	//為了轉型弄的 int[] to ulong
	//int[] to StringStream to string to ulong
	std::string sstr;
	std::stringstream ss;
	for (int i = 0; i < (int)(strlen((char*)buf)); i++) {
		ss << buf[i];
	}
	sstr = ss.str();
	ULONG temp = std::stoul(sstr, nullptr);
	//set family port addr
	sock.sin_family = PF_INET;   //IPV4
	sock.sin_addr.s_addr = temp; //設定IP
	sock.sin_port = htons(port);  //設定PORT

	return sock;
}


int socklink::get_port() {
	return port;
}


char* socklink::get_ip() {
	return ip;
}


std::string socklink::get_sip() {
	return sip;
}