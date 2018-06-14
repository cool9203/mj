#include "Player.h"

Player::Player() {

}


Player::~Player() {

}


void Player::wait() {
	while (true) {
		json r;
		sock->read(r);
		if (r.is_null())
			continue;
		if (r.find("done") != r.end())
			break;
	}
}


json Player::read() {
	json j,d;
	d["done"];
	while (true) {
		sock->read(j);
		if (j.is_null())
			continue;
		sock->send(d);
		break;
	}
	return j;
}


void Player::sendtoserver() {

}


void Player::close() {

}


void Player::start() {
	//link dll
	WSADATA wsaData;
	int wsad = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsad != 0) {
		std::cout << "link dll failed." << std::endl;
		system("PAUSE");
		return;
	}
	//create socket
	int socketfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketfd == INVALID_SOCKET)
	{
		std::cout << "create socketfd failed.\n";
		return;
	}

	sock = new clientsocket(socketfd, "127.0.0.1", 9990);

	int c = sock->connect();
	if (c != 0) {
		switch (c) {
		case 899:
			std::cout << "create SOCKET failed.\n";
			break;
		default:
			std::cout << "client connect failed.\n";
			break;
		}
		return;
	}


	mjcard card;
	bool who = false;
	while (true) {
		bool turn = false;
		json r;
		r = read();

		for (json::iterator it = r.begin(); it != r.end(); ++it) {
			//std::cout << it.key() << " : " << it.value() << "\n";
			if (it.key() == "sendcard") {
				card.push(it.value());
				card.arrange();
			}
			else if (it.key() == "youplayernumber") { //紀錄自己的玩家編號
				player_number = it.value();
			}
			else if (it.key() == "gamestart") { //遊戲開始
				//std::cout << "player[" << player_number << "]:gamestart\n";
			}
			else if (it.key() == "sendoutcard") { //你要丟牌丟牌
				//丟牌
				json out;
				out["sendoutcard"] = card.out();
				//std::cout << "playerout:" << out << "\n\n";
				sock->send(out);
				wait();
			}
			else if (it.key() == "endgame") {
				who = true;
				break;
			}
			else if (it.key()=="eat") {
				int outcard = it.value();
				card.set_need_card();
				std::vector<int> tempcard;
				tempcard = card.need(outcard);
				if (tempcard.size() != 0) {
					json mydo;
					if (card.get_need_status() == "eat") {
						mydo = iarrtojson(tempcard, "do", "card");
					}
					else {
						mydo["no"];
					}
					sock->send(mydo);
					wait();
				}else{
					json mydo;
					mydo["no"];
					sock->send(mydo);
					wait();
				}
			}
			else if (it.key() == "pung") {
				int outcard = it.value();
				card.set_need_card();
				std::vector<int> tempcard;
				tempcard = card.need(outcard);
				if (tempcard.size() != 0) {
					json mydo;
					if (card.get_need_status() == "pung") {
						mydo = iarrtojson(tempcard, "do", "card");
					}
					else {
						mydo["no"];
					}
					sock->send(mydo);
					wait();
				}
				else {
					json mydo;
					mydo["no"];
					sock->send(mydo);
					wait();
				}
			}
			else if (it.key() == "who") {
				json j;
				j["do"];
				sock->send(j);
				wait();
			}
			else if (it.key() == "eatcard") {
				std::vector<int> tempcard;
				int doplayer = getjsonvalue(r, tempcard, 3, "eatcard");
				//std::cout << "player" << doplayer << " eat.\n";
				if (doplayer == player_number) {
					card.get(tempcard.at(0), tempcard.at(1), tempcard.at(2));
					turn = true;
				}
				else {
					card.set_mycardtimes(tempcard);
				}
			}
			else if (it.key() == "pungcard") {
				std::vector<int> tempcard;
				int doplayer = getjsonvalue(r, tempcard, 3, "pungcard");
				//std::cout << "player" << doplayer << " pung.\n";
				if (doplayer == player_number) {
					card.get(tempcard.at(0), tempcard.at(1), tempcard.at(2));
					turn = true;
				}
				else {
					card.set_mycardtimes(tempcard);
				}
			}
			else if (it.key() == "whocard") {
				std::vector<int> tempcard;
				int whoplayer = getjsonvalue(r, tempcard, 17, "whocard");
				//std::cout << "player[" << whoplayer << "] Win:\n";

				if (whoplayer != player_number) {
					/*for (std::vector<int>::iterator it = tempcard.begin(); it != tempcard.end(); ++it)
						std::cout << *it << " ";
					std::cout << "\n";*/
				}
					
				who = true;
				break;
			}
		}
		if (who) {
			//std::cout << "client get end.\n";
			break;
		}
	}
}


/*
void Player::start() {
	//link dll
	WSADATA wsaData;
	int wsad = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsad != 0) {
		std::cout << "link dll failed." << std::endl;
		system("PAUSE");
		return;
	}
	//create socket
	int socketfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketfd == INVALID_SOCKET)
	{
		std::cout << "create socketfd failed.\n";
		return;
	}

	sock = new clientsocket(socketfd, "127.0.0.1", 9990);

	int c = sock->connect();
	if (c != 0) {
		switch (c) {
		case 899:
			std::cout << "create SOCKET failed.\n";
			break;
		default:
			std::cout << "client connect failed.\n";
			break;
		}
		return;
	}


	mjcard card;
	int outcard = 0, outplayer = -1;
	bool who = false;
	while (true) {
		bool turn = false;
		json r;
		r = read();

		for (json::iterator it = r.begin(); it != r.end(); ++it) {
			//std::cout << it.key() << " : " << it.value() << "\n";
			if (it.key() == "sendcard") {
				card.push(it.value());
			}
			else if (it.key() == "youplayernumber") { //紀錄自己的玩家編號
				player_number = it.value();
			}
			else if (it.key() == "gamestart") { //遊戲開始
				//std::cout << "player[" << player_number << "]:gamestart\n";
			}
			else if (it.key() == "youturn") { //玩家回合 要丟牌
				turn = true;
			}
			else if (it.key() == "outcard") { //其他玩家丟牌
				std::vector<int> tempoutcard;
				outplayer = getjsonvalue(r, tempoutcard, 1, "outcard");
				outcard = tempoutcard.at(0);
			}
			else if (it.key() == "endgame") {
				std::cout << "流局\n";
				who = true;
				break;
			}
			else if (it.key() == "do") {
				sendpolling(card, outcard);
			}
			else if (it.key() == "eatcard") {
				std::vector<int> tempcard;
				int doplayer = getjsonvalue(r, tempcard, 3, "eatcard");
				//std::cout << "player" << doplayer << " eat.\n";
				if (doplayer == player_number) {
					card.get(tempcard.at(0), tempcard.at(1), tempcard.at(2));
					turn = true;
				}
				else {
					card.set_mycardtimes(tempcard);
				}
			}
			else if (it.key() == "pungcard") {
				std::vector<int> tempcard;
				int doplayer = getjsonvalue(r, tempcard, 3, "pungcard");
				//std::cout << "player" << doplayer << " pung.\n";
				if (doplayer == player_number) {
					card.get(tempcard.at(0), tempcard.at(1), tempcard.at(2));
					turn = true;
				}
				else {
					card.set_mycardtimes(tempcard);
				}
			}
			else if (it.key() == "whocard") {
				std::vector<int> tempcard;
				int whoplayer = getjsonvalue(r, tempcard, 17, "whocard");
				std::cout << "player[" << whoplayer << "] Win:\n";
				
				for (std::vector<int>::iterator it = tempcard.begin(); it != tempcard.end(); ++it)
					std::cout << *it << " ";
				std::cout << "\n";
				
				who = true;
				break;
			}
		}

		if (turn) {
			if (card.check_who()) {
				std::vector<int> tempcard;
				card.print();
				std::cout << "你胡牌了. playnumber:" << player_number << "\n\n";
				tempcard = card.get_card();
				json mywho;
				mywho = iarrtojson(tempcard, "who", "card");
				sock->send(mywho);
				continue;
			}
			//丟牌
			json out;
			out["outcard"] = card.out();
			//std::cout << "playerout:" << out << "\n\n";
			sock->send(out);
			wait();
			//整理牌
			card.arrange();
		}
		if (who) {
			//std::cout << "client get end.\n";
			break;
		}
	}
}*/