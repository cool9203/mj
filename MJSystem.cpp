//牌局系統-MJSystem
//沒有起始的骰子概念、也沒有時間的概念
#include "MJSystem.h"
#include <random>

MJSystem::MJSystem() {

}


MJSystem::~MJSystem() {
	close();
}


void MJSystem::close() {
	server->close();
	endclient();
}


//清除vector<> client的動態配置記憶體
void MJSystem::endclient() {
	for (int i = 0; i < static_cast<int>(client.size()); i++) {
		delete client[i];
	}
	client.clear();
}


void MJSystem::getclient() {
	//get client into vector
	for (int i = 0; i < 4; i++) {
		clientsocket *c = server->accept();
		if (!c->status()) {
			delete c;
			i--;
			continue;
		}
		json j;
		j["youplayernumber"] = i;
		c->send(j);
		client.push_back(c);
		wait(i);
	}
}


//洗牌
void MJSystem::card_rand() {
	card_push(1, 9);
	card_push(11, 19);
	card_push(21, 29);

	for (int i = 31; i <= 43; i = i + 2) {
		card_push(i, i);
	}
	//再洗牌Q_Q
	std::random_device rd; //利用硬體系統給的值來取值
	std::default_random_engine generator(rd()); //初始化，並利用rd()的值找出相對應的種子碼??
	std::uniform_int_distribution<int> distribution(0, 135); //設定範圍與決定要產生什麼類型的亂數 ex:int float..

	for (int i = 0; i < 136; i++) {
		std::swap(card.at(i), card.at(distribution(generator))); //呼叫亂數則使用上面2個的變數來呼叫、使用
	}
}


void MJSystem::card_push(int start, int end) {
	for (int i = start; i <= end; i++) {
		for (int j = 0; j < 4; j++) {
			card.push_back(i);
		}
	}
}


int MJSystem::get_card() {
	if (card.size() != 16) {
		int data = card.at(0);
		card.erase(card.begin());
		return data;
	}
	return -1;
}


int MJSystem::check_eat(int card1,int card2,int getcard) {
	int temparr[3] = { card1,card2,getcard };
	card_sort(temparr, 3, true);
	if (temparr[0] + 1 == temparr[1] && temparr[1] + 1 == temparr[2])
		return 1;
	return 0;
}


int MJSystem::check_pung(int card1, int card2, int getcard) {
	if (card1 == card2 && card1 == getcard)
		return 1;
	return 0;
}


void MJSystem::wait(int assign) {
	while (true) {
		json r;
		client.at(assign)->read(r);
		if (r.is_null())
			continue;
		if (r.find("done") != r.end())
			break;
	}
}


json MJSystem::read(int assign) {
	json j, d;
	d["done"];
	while (true) {
		client.at(assign)->read(j);
		if (j.is_null())
			continue;
		//std::cout << "server get  :" << j << "\n\n";
		client.at(assign)->send(d);
		break;
	}
	return j;
}


void MJSystem::sendtoclient(json j) {
	for (int i = 0; i < 4; i++) {
		client.at(i)->send(j);
		wait(i);
	}
}


void MJSystem::sendtoclient(const char *name) {
	json j;
	j[name];
	sendtoclient(j);
}



void MJSystem::sendtoclient(const char *name, int data) {
	json j;
	j[name] = data;
	sendtoclient(j);
}


void MJSystem::sendtoclient(std::vector<int> data, const char *main_json_name, const char *second_json_name,int playernumber) {
	json j = iarrtojson(data, main_json_name, second_json_name, true, playernumber);
	sendtoclient(j);
}


void MJSystem::sendtoclient_one(json j, int assign) {
	client.at(assign)->send(j);
	wait(assign);
}


void MJSystem::sendtoclient_one(const char *name, int data, int assign) {
	json j;
	j[name] = data;
	sendtoclient_one(j, assign);
}


void MJSystem::sendoutcard(int outcard, int main_player) {
	for (int i = 0; i < 4; i++) {
		if (i == main_player)
			continue;

		json j, secj;
		secj["playernumber"] = main_player;
		secj[getstri("card", 1)] = outcard;
		j["outcard"] = secj;
		client.at(i)->send(j);
		wait(i);

	}
}



void MJSystem::start() {
	//link dll
	WSADATA wsaData;
	int wsad = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsad != 0) {
		std::cout << "link dll failed." << std::endl;
		system("PAUSE");
		return;
	}

	int socketfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketfd == INVALID_SOCKET)
	{
		return;
	}

	//create ServerSocket
	server = new myserversocket(socketfd, 9990, 10, "127.0.0.1");
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

	int main_player = 0;
	std::vector<int> whocard;
	bool who = false;
	//get 4 client and send playernumber
	getclient();

	//send json["gamestart"]
	sendtoclient("gamestart");
	//card random
	card_rand();
	//send 4*4card to player
	for (int i = 0; i < 4; i++) {
		for (int player = 0; player < static_cast<int>(client.size()); player++) {
			for (int count = 0; count < 4; count++) {
				sendtoclient_one("sendcard", get_card(), player);
			}
		}
	}
	//game start
	while (true) {
		int outcard = -1;
		if (card.size() == 16) {
			std::cout << "endgame.\n";
			sendtoclient("endgame");
			break;
		}

		//send to mainplayer(莊家) one card and notice he turn.  and get mainplayer's outcard or he self who.
		for (int i = 0; i < 1; i++) {
			json j, r;
			//sned one card and notice he turn.
			j["youturn"];
			j["sendcard"] = get_card();
			sendtoclient_one(j, main_player);

			r = read(main_player);

			for (json::iterator it = r.begin(); it != r.end(); ++it) {
				if (it.key() == "outcard") { //如果是丟牌
					outcard = it.value();
					break;

				}
				else if (it.key() == "who") { //如果他自摸
					getjsonvalue(r, whocard, 17, false, "who");
					if (card_check(whocard) != 0) {
						//std::cout << "server get who.\n";
						who = true;
					}
					break;
				}
			}
		}

		//one check he self who
		if (who == true) { //自摸成立
			break;
		}

		while (true) { //連續吃碰槓while
			int player_do = -1;
			//先傳main_player丟的牌給其他3位玩家
			sendoutcard(outcard, main_player);
			//輪詢-問各個使用者是否需要作事(吃碰胡)
			json r;
			//先問各個玩家想執行的動作,並用vector client[index]紀錄下來
			for (int i = 1; i < 4; i++) {
				json tr;
				int number = (main_player + i) % 4;
				sendtoclient_one("do", NULL, number);
				tr = read(number);
				if (tr.find("who") != tr.end()) {
					r.clear();
					r = tr;
					player_do = number;
					break;
				}
				else if (tr.find("pung") != tr.end() && r.find("who") == r.end()) {
					player_do = number;
					r.clear();
					r = tr;
				}
				else if (tr.find("eat") != tr.end() && r.is_null()) {
					player_do = number;
					r.clear();
					r = tr;
				}
			}
			//std::cout << r << "\n\n";
			//找完吃碰槓胡的順序後，決定下個main_player和檢查client傳過來的資訊，來確定是否是正確的動作
			//有人胡牌
			if (r.find("who") != r.end()) {
				getjsonvalue(r, whocard, 16, "who");
				whocard.push_back(outcard);
				if (card_check(whocard)) {
					main_player = player_do;
					who = true;
					std::cout << "player[" << main_player << "]:who\n\n";
					break;
				}
				else
					player_do = -1;
			}//碰牌
			else if (r.find("pung")!=r.end()) {
				std::vector<int> tempcard;
				getjsonvalue(r, tempcard, 2, "pung");
				std::cout << "pungcard:" << tempcard.at(0) << " " << tempcard.at(1) << " " << outcard << std::endl;
				if (check_pung(tempcard.at(0), tempcard.at(1), outcard)) {
					main_player = player_do;
					std::cout << "player[" << main_player << "]:pung\n\n";
					tempcard.push_back(outcard);
					sendtoclient(tempcard, "pungcard", "card", main_player);
				}
				else
					player_do = -1;
			}//吃牌
			else if (r.find("eat") != r.end()) {
				std::vector<int> tempcard;
				getjsonvalue(r, tempcard, 2, "eat");
				std::cout << "eatcard:" << tempcard.at(0) << " " << tempcard.at(1) << " " << outcard << std::endl;
				if (check_eat(tempcard.at(0), tempcard.at(1), outcard)) {
					main_player = player_do;
					std::cout << "player[" << main_player << "]:eat\n\n";
					tempcard.push_back(outcard);
					sendtoclient(tempcard, "eatcard", "card", main_player);
				}
				else
					player_do = -1;
			}

			if (player_do >= 0) {
				//read "outcard"
				json temprj;
				while (true) {
					temprj = read(main_player);
					if (temprj.find("outcard") != temprj.end())
						break;
					std::cout << temprj << "\n\n";
				}
				
				outcard = static_cast<int>(temprj["outcard"]);
			}
			else { //都沒事,下個使用者
				main_player = (main_player + 1) % 4;
				break;
			}
		}//連續吃碰槓while's
	}//this is playing while's
	try {
		//處理胡牌
		if (who == true) {
			sendtoclient(whocard, "whocard", "card", main_player);
		}
	}
	catch (...) {
		std::cout << "get exception.\n";
		for (int i = 0; i < 17; i++)
			std::cout << whocard[i] << " ";
	}
}
