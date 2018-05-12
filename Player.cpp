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


void Player::sendpolling() {
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
				std::cout << "player[" << player_number << "]:gamestart\n";
			}
			else if (it.key() == "youturn") { //玩家回合 要丟牌
				turn = true;
			}
			else if (it.key() == "outcard") { //其他玩家丟牌
				int tempoutcard;
				outplayer = getjsonvalue(r, &tempoutcard, 1, true, "outcard");
				outcard = tempoutcard;
			}
			else if (it.key() == "endgame") {
				std::cout << "流局\n";
				who = true;
				break;
			}
			else if (it.key() == "do") {
				sendpolling();
			}
			else if (it.key() == "eatcard") {
				int tempcard[3];
				int doplayer = getjsonvalue(r, tempcard, 3, true, "eatcard");
				std::cout << "player" << doplayer << " eat.\n";
				if (doplayer == player_number) {
					card.get(tempcard[0], tempcard[1], tempcard[2]);
				}
				else {
					card.set_mycardtimes(tempcard, 3);
				}
			}
			else if (it.key() == "pungcard") {
				int tempcard[3];
				int doplayer = getjsonvalue(r, tempcard, 3, true, "pungcard");
				std::cout << "player" << doplayer << " pung.\n";
				if (doplayer == player_number) {
					card.get(tempcard[0], tempcard[1], tempcard[2]);
				}
				else {
					card.set_mycardtimes(tempcard, 3);
				}
			}
			else if (it.key() == "whocard") {
				int tempcard[17];
				int whoplayer = getjsonvalue(r, tempcard, 17, true, "whocard");
				std::cout << "player[" << whoplayer << "] Win:";
				/*
				for (int i = 0; i < 17; i++)
					std::cout << tempcard[i] << " ";
				std::cout << "\n";
				*/
				who = true;
				break;
			}
		}

		if (turn) {
			if (card.check_who()) {
				card.print();
				std::cout << "你胡牌了. playnumber:" << player_number << "\n\n";
				json mywho;
				mywho = iarrtojson(card.get_card(), 17, "who", "card");
				sock->send(mywho);
				continue;
			}
			//丟牌
			json out;
			out["outcard"] = card.out();
			sock->send(out);
			wait();
			//整理牌
			card.arrange();
		}
		if (who) {
			std::cout << "client get end.\n";
			break;
		}
	}
}


int mydo(int *card, int cardlen, int *data, int datalen) {
	int cptr = cardlen;
	for (int i = 0; i < datalen - 1; i++) {
		int size = cptr;
		for (int j = 0; j < size; j++) {
			if (card[j] == data[i]) {
				std::swap(card[cptr], card[cptr - 1]);
				std::swap(card[cptr], card[j]);
				cptr--;
				break;
			}
		}
	}

	card[cptr] = data[datalen - 1];
	cptr--;
	return cptr;
}


//如果server傳 [do] 我要做什麼事
/*
void sendpolling(clientsocket &sock,int outcard,int outplayer) {
		
		//處理胡牌
		int whocard[17];
		card_cpy(card, whocard, 17);
		whocard[cptr] = outcard;
		if (card_check(whocard, 17)) {
			std::cout << "你胡到別人牌了!\n";
			json j, secj;
			for (int i = 0; i < 17; i++) {
				if (i == cptr)
					continue;

				secj[getstri("card", i + 1 - (i%cptr))] = whocard[i];
			}
			j["who"] = secj;
			sock.send(j);
			wait(sock);
			return;
		}

		//處理吃碰
		for (int i = 0; i < static_cast<int>(need_card.size());i++) {
			json j;
			try {
				if (outcard == need_card.at(i)) { //別人丟的牌是我要吃碰的牌
					if (card[need_card_index.at(i * 2)] == card[need_card_index.at(i * 2 + 1)]) { //如果是碰的話
						std::cout << "player[" << player_number << "] want to pung.\n\n";
						int temparr[2] = { card[need_card_index.at(i * 2)] ,card[need_card_index.at(i * 2 + 1)] };
						j = iarrtojson(temparr, 2, "pung", "card");
						sock.send(j);
						wait(sock);
					}
					else { //不是碰那就是吃
						std::cout << "player" << player_number << ":outplayer = " << outplayer << std::endl;
						if (outplayer != (player_number - 1) % 4)
							break;
						std::cout << "player[" << player_number << "] want to eat.\n\n";
						int temparr[2] = { card[need_card_index.at(i * 2)] ,card[need_card_index.at(i * 2 + 1)] };
						j = iarrtojson(temparr, 2, "eat", "card");
						sock.send(j);
						wait(sock);
					}
					return;
				}
			}
			catch(...){
				std::cout << "i=" << i << std::endl;
				std::cout << "need=" << need_card.size() << std::endl;
				std::cout << "need_index=" << need_card_index.size() << std::endl;
			}
			
		}
		
		//如果都不能做
		json j;
		j["no"];
		sock.send(j);
		wait(sock);
	}
*/

//Player Start
/*void start() {
		//create socket
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
				std::cout << "create SOCKET failed.\n";
				return;
			default:
				std::cout << "\tclient connect failed.\n";
				return;
			}
		}
		int cptr = 0;
		int outcard, outplayer;
		bool who = false;
		bool setcardtimes = false;

		//根據讀到的json來作出指定的事情
		while (true) {
			bool turn = false;
			json j, d, s;
			d["done"];

			//先讀取json,如果沒有讀到東西 than continue
			sock.read(j);
			if (j.is_null()) {
				continue;
			}
			
			sock.send(d);//有東西了,先send d[done],向系統表示收到了

			for (json::iterator it = j.begin(); it != j.end(); ++it) {
				//std::cout << it.key() << " : " << it.value() << "\n";
				if (it.key() == "sendcard") {
					card[cptr] = it.value();
					set_mycardtimes(card[cptr]);
					//std::cout << "get card:" << card[cptr] << std::endl;
					cptr++;
					//std::cout << "player[" << player_number << "]:" << "cptr=" << cptr << std::endl;

				}
				else if (it.key() == "do") { //要作出回應,看要吃碰槓胡,如果都沒事就sned json[no]
					get_need_card(card, cptr);
					sendpolling(sock, outcard, outplayer);

				}
				else if (it.key() == "youplayernumber") { //紀錄自己的玩家編號
					player_number = it.value();

				}
				else if (it.key() == "gamestart") { //遊戲開始
					std::cout << "player[" << player_number << "]:gamestart\n";

				}
				else if (it.key() == "youturn") { //玩家回合 要丟牌
					turn = true;

				}
				else if (it.key() == "outcard") { //其他玩家丟牌
					int tempoutcard[2];
					outplayer = getjsonvalue(j, tempoutcard, 1, true, "outcard");
					outcard = tempoutcard[0];
					set_mycardtimes(outcard);

				}
				else if (it.key() == "eatcard") { //其他玩家吃牌
					int eatcard[3];
					int eatplayernumber = getjsonvalue(j, eatcard, 3, true, "eatcard");
					std::cout << "player" << eatplayernumber << " eat.\n";
					if (eatplayernumber == player_number) {
						turn = true;
						cptr = mydo(card, cptr, eatcard, 3);
					}

				}
				else if (it.key() == "pungcard") { //其他玩家碰牌
					int pungcard[3];
					int pungplayernumber = getjsonvalue(j, pungcard, 3, true, "pungcard");
					std::cout << "player" << pungplayernumber << " pung.\n";
					if (pungplayernumber == player_number) {
						turn = true;
						cptr = mydo(card, cptr, pungcard, 3);
					}

				}
				else if (it.key() == "whocard") { //其他玩家胡牌
				   //end
					//std::cout << "client get who.\n";
					int buf[17], number = -1;
					number = getjsonvalue(j, buf, 17, true, "whocard");
					std::cout << "player[" << number << "] Win:";
					who = true;
					//printcard(buf, 17);
					print(buf, 17);
					std::cout << "\n\n";
					break;

				}
				else if (it.key() == "endgame") {
					std::cout << "流局\n";
					break;
				}
					
			}

			if (turn == true) { //拉出來做,才不會json[youturn]不是在最後導致牌沒拿完就準備丟牌
				//print(card, 17);
				//確認是否胡牌
				if (card_check(card, cptr)) { //胡牌了
					s = iarrtojson(card, 17, "who", "card");
					//std::cout << "client who s:" << s << "\n\n";
					sock.send(s);
					wait(sock);
					std::cout << "你胡牌了. playnumber:" << player_number << "\n\n";
					continue;
				}

				//整牌
				cptr = card_organize(card, cptr);
				card_sort(card, cptr, true);

				//丟牌
				
				int index = card_price(card, cptr);
				int myoutcard = card[index];
				card[index] = -1;
				swap(card[cptr - 1], card[index]); //swap，讓進牌可以在最後面進
				cptr--;
				json sout;
				sout["outcard"] = myoutcard;
				sock.send(sout);
				std::cout << "outcard\n";
				wait(sock);
			}

			if (who) {
				break;
			}

		}

		sock.close();
		return;
	}*/









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