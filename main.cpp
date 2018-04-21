#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <mutex>
#include <windows.h>
#include <sstream>
#include <string>
#include <iostream>
#include "MJsystem.cpp"
#include "Player.cpp"
#include "myserversocket.h"
#include "mylib\single_include\nlohmann\json.hpp"
#pragma comment(lib, "Ws2_32.lib")

std::mutex mtx;

void getclient(clientsocket *);
void client();
void server();

//get std::string(name+intdata)
std::string getstr(const char *name, int intdata) {
	std::string str = name, temp;
	std::stringstream ss;
	ss << intdata;
	ss >> temp;
	str += temp;

	return str;
}

int getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname, const char *getname) {
	json j = tj[tjname];
	int playernumber = -1;

	if (getplayernumber && j.find("playernumber") != j.end()) { //�n���ϥΪ̽s�� �P �u�����ǨϥΪ̽s�� than
		playernumber = j["playernumber"];
	}

	for (int i = 0; i < getlen; i++) {
		buf[i] = j[getstr(getname, i + 1)];
	}

	return playernumber;
}

//return playernumber
int getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname) {
	return getjsonvalue(tj, buf, getlen, getplayernumber, tjname, "card");
}

json iarrtojson(int *data, int datalen, const char *main_json_name, const char *second_json_name, bool sendplayernumber, int playernumber) {
	json j, secj;

	if (sendplayernumber && playernumber) {
		secj["playernumber"] = playernumber;
	}
	else if (sendplayernumber && playernumber <= 0) {
		throw "player_number_error";
	}

	for (int i = 0; i < datalen; i++) {
		secj[getstr(second_json_name, i + 1)] = data[i];
	}
	j[main_json_name] = secj;

	return j;
}
//int to json
json iarrtojson(int *data, int datalen, const char *main_json_name, const char *second_json_name) {
	return iarrtojson(data, datalen, main_json_name, second_json_name, false, -1);
}






int main(int argc, char *argv[]) {
	//link dll
	WSADATA wsaData;
	int wsad = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsad != 0) {
		std::cout << "link dll failed." << std::endl;
		system("PAUSE");
		return 0;
	}
	while (true) {
		try {
			std::thread *s = new std::thread(&MJ::MJSystem::start, MJ::MJSystem()); //https://stackoverflow.com/questions/10673585/start-thread-with-member-function


			std::thread *c1 = new std::thread(&Player::start, Player());
			std::thread *c2 = new std::thread(&Player::start, Player());
			std::thread *c3 = new std::thread(&Player::start, Player());
			std::thread *c4 = new std::thread(&Player::start, Player());

			s->join();
		}
		catch (...) {
			abort();
		}

		int p;
		std::cin >> p;

		if (p == 1)
			break;
	}
	
	
	

	system("PAUSE");

	return 0;
}


void wait(clientsocket &sock) {
	while (true) {
		json r;
		sock.read(r);
		if (r.find("done") != r.end())
			break;
	}
}


void client() {
	int socketfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketfd == INVALID_SOCKET)
	{
		return;
	}

	clientsocket sock(socketfd, "127.0.0.1", 9990);

	int c = sock.connect();
	if (c != 0) {
		switch (c) {
		case 899:
			std::cout << "create SOCKET failed." << std::endl;
			return;
		default:
			std::cout << "client connect failed." << std::endl;
			return;
		}
	}
	int card[17] = { -1 };
	int cptr = 0;
	int player_number;

	//�ھ�Ū�쪺json�ӧ@�X���w���Ʊ�
	while (true) {
		bool turn = false;
		json j,d,s;
		d["done"];

		//��Ū��json,�p�G�S��Ū��F�� than continue
		sock.read(j);
		if (j.size() == 0)
			continue;
		sock.send(d);//���F��F,��send d[done],�V�t�Ϊ�ܦ���F

		mtx.lock();
		for (json::iterator it = j.begin(); it != j.end(); ++it) {
			//std::cout << it.key() << " : " << it.value() << "\n";
			if (it.key() == "sendcard") {
				card[cptr] = it.value();
				cptr++;
			}else if (it.key() == "playernumber") { //�����ۤv�����a�s��
				player_number = it.value();

			}else if (it.key() == "gamestart") { //�C���}�l
				std::cout << "player[" << player_number << "]:gamestart\n";

			}else if (it.key() == "youturn") { //���a�^�X �n��P
				turn = true;

			}else if (it.key() == "do") { //�n�@�X�^��,�ݭn�Y�I�b�J,�p�G���S�ƴNsned json[no]

			}
		}

		if (turn == true) { //�ԥX�Ӱ�,�~���|json[youturn]���O�b�̫�ɭP�P�S�����N�ǳƥ�P
			//�T�{�O�_�J�P
			
			//��P

			//��P


		}

		mtx.unlock();
	}


	//example cout key and value
	//for (json::iterator it = j.begin(); it != j.end(); ++it) {
		//std::cout << it.key() << " : " << it.value() << "\n";
	//}

}

void getclient(clientsocket *client) {
	//std::cout << "Get Client." << std::endl;
	socklink *addr = client->get_socketaddress();

	mtx.lock();
	std::cout << "server get client. from ip:" << addr->get_ip() << ":" << addr->get_port() << std::endl;
	mtx.unlock();

	char buf[1024] = "";

	client->send("this is Server.");

	client->read(buf, 1024);
	mtx.lock();
	for (int i = 0; i < (int)(strlen(buf)); i++) {
		std::cout << buf[i];
	}

	std::cout << std::endl << std::endl;


	std::cout << "close client from ip:" << addr->get_ip() << ":" << addr->get_port() << std::endl;
	mtx.unlock();
	delete client;

}


/*
void server() {

	int socketfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketfd == INVALID_SOCKET)
	{
		return;
	}

	myserversocket *server = new myserversocket(socketfd, 9999, 10, "127.0.0.1");
	int l = server->listen();
	if (l != 1) {
		switch (l) {
		case -1:
			std::cout << "create SOCKET failed." << std::endl;
			return;
		case -2:
			std::cout << "bind failed." << std::endl;
			return;
		case -3:
			std::cout << "listen failed." << std::endl;
			return;
		}
	}

	while (true) {
		clientsocket *client = server->accept();
		if (!client->status()) {
			delete client;
			continue;
		}

		std::thread *t = new std::thread(getclient, client);
	}
}
*/