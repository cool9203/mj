#pragma once
#include "mjtransmission.h"
#include "mjcard.h"
#include "clientsocket.h"
#include "mylib\single_include\nlohmann\json.hpp"
using json = nlohmann::json;

class Player : public mjtransmission {
protected:
	int player_number;
	clientsocket *sock;

	void wait();
	json read();

	void sendtoserver();
public:
	Player();
	~Player();
	void close();

	void start();


};