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


void Player::sendpolling(mjcard &card, int outcard) {
	if (card.check_who(outcard) == true) {
		std::vector<int> tempcard;
		std::cout << "你胡別人牌了. playnumber:" << player_number << "\n\n";
		tempcard = card.get_card();
		json mywho;
		mywho = iarrtojson(tempcard, "who", "card");
		sock->send(mywho);
		return;
	}
	card.set_need_card();
	std::vector<int> tempcard;
	tempcard = card.need(outcard);
	if (tempcard.size() != 0) {
		json mydo;
		if (card.get_need_status() == "eat") {
			mydo = iarrtojson(tempcard, "eat", "card");
		}
		else if (card.get_need_status() == "pung") {
			mydo = iarrtojson(tempcard, "pung", "card");
		}
		sock->send(mydo);
		return;
	}

	//如果都不能做
	json j;
	j["no"];
	sock->send(j);
	wait();
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
}


//test
/*
int main() {
#pragma region 詐&胡牌測試資料
	//int card[17] = { 11,11,11,16,6,7,7,8,9,16,1,3,3,2,4,5,5 }; //0 1 2 3 4 5 6 7 8 9 10 11 真
	//int card[17] = {1,1,1,1,2,3,5,5,5,6,7,14,15,16,17,18,19}; //0 4 5 6 9 10 11 12 13 14 15 16 真
	//int card[17] = {1,2,3,4,5,6,7,8,9,21,21,22,22,23,23,24,24}; //0 1 2 3 4 5 6 7 8 9 11 13 10 12 14 真
	//int card[17] = {1,1,2,2,3,3,4,4,5,5,6,6,41,41,41,37,37}; //0 2 4 1 3 5 6 8 10 7 9 11 真
	//int card[17] = {1,1,1,2,2,2,3,3,3,11,12,13,14,15,16,16,16}; //0 3 6 1 4 7 2 5 8 9 10 11 12 13 14 真
	//int card[17] = {1,2,3,4,5,7,7,7,9,9,9,11,11,11,17,18,19}; //0 1 2 14 15 16 詐
	//int card[17] = {1,2,3,4,5,6,7,8,9,21,21,22,23,23,27,28,29}; //0 1 2 3 4 5 6 7 8 9 11 12 14 15 16 詐
	//int card[17] = {6,7,8,12,13,14,15,16,17,25,26,26,26,26,41,41,41}; //0 1 2 3 4 5 6 7 8  詐
	//int card[17] = {6,7,8,12,13,14,15,16,17,25,26,26,26,27,41,41,41}; //0 1 2 3 4 5 6 7 8 9 10 13 真
	//int card[17] = {1,1,1,3,3,3,5,5,5,7,7,7,9,9,9,11,11}; //null 真
	//int card[17] = {1,2,3,3,3,4,5,6,6,6,6,7,8,9,9,9,9}; // 真
	//int card[17] = { 1,3,5,7,9,11,14,17,22,25,28,33,36,39,41,43,45 }; // 詐
	//int card[17] = {3,4,8,9,6,7};
#pragma endregion</span>
	
	Player p1;
	MJSystem mj;
	//p1.set_mycardtimes(card, 17);
	//p1.start();

	json j;
	string str = "{\"haha\":123,\"papa\":456}";
	j = json::parse(str);
	//取得json的 key & value
	for (json::iterator it = j.begin(); it != j.end(); ++it) {
		std::cout << it.key() << " : " << it.value() << "\n";
	}
	

	
	//test胡牌是否正確
	/*if (p1.card_check(card, 17) == 1) {
		cout << "胡牌";
	}
	else {
		cout << "詐胡";
	}
	cout << endl;

	system("PAUSE");
	
}*/