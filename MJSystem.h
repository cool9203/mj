#pragma once
#include <vector>
#include "mjtransmission.h"
#include "myserversocket.h"
#include "mylib\single_include\nlohmann\json.hpp"
using json = nlohmann::json;

class MJSystem : public mjtransmission {
protected:
	myserversocket *server;
	std::vector<clientsocket*> client;
	std::vector<int> card;

	
	void getclient();

	void card_rand();
	void card_push(int start, int end);
	int get_card();
	int check_eat(int,int,int);
	int check_pung(int,int,int);

	void wait(int);

	json read(int assign);
	
	void sendtoclient(json j);
	void sendtoclient(const char *name);
	void sendtoclient(const char *name, int data);
	void sendtoclient(int *data, int datalen, const char *main_json_name, const char *second_json_name, int playernumber);

	void sendtoclient_one(json j, int assign);
	void sendtoclient_one(const char *name, int data, int assign);

	void sendoutcard(int outcard, int main_player);
public:
	MJSystem();
	~MJSystem();
	void close();
	void endclient();

	void start();
	

};