#undef _WINSOCKAPI_
#define _WINSOCKAPI_

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <Windows.h>
#include "mylib\single_include\nlohmann\json.hpp"
#include "MJSystem.cpp"
#include "clientsocket.h"
using json = nlohmann::json;


class Player {
private:
	int card[17] = { 0 };
	int whocard[17] = { 0 };
	int player_number = -1;
	int card_times[44]; //牌的出現機率
	std::vector<int> gun_card, need_card, need_card_index, out_card;

	int cptr = 0;
	int who_cptr = 0;

public:
	Player() {
		for (int i = 0; i < 17; i++) {
			card[i] = 0;
		}
		for (int i = 0; i < 44; i++) {
			card_times[i] = 4;
		}
	}
	//向系統要牌



	//丟牌



	//吃碰槓


	//print card to chinese
	void printcard(int *card,int cardlen) {
		std::string stack;
		std::string dcm = "萬條筒風中財板";
		std::string chinesemath = "一二三四五六七八九東南西北紅發白";
		std::cout << "\n";
		for (int i = 0; i < cardlen; i++) {
			char c[3];
			if (card[i] < 30) {
				c[0] = chinesemath.at((card[i] % 10 - 1) * 2);
				c[1] = chinesemath.at((card[i] % 10 - 1) * 2 + 1);
				c[2] = '\0';
			}
			else {
				c[0] = chinesemath.at((card[i] % 31) + 18);
				c[1] = chinesemath.at((card[i] % 31) + 19);
				c[2] = '\0';
			}

			std::cout << c << "  ";
			
			if (card[i] < 10) {
				stack.push_back(0);
			}
			else if (card[i] < 20 && card[i] > 10) {
				stack.push_back(2);
			}
			else if (card[i] < 30 && card[i] > 20) {
				stack.push_back(4);
			}
			else if (card[i] < 38 && card[i] > 30) {
				stack.push_back(6);
			}
			else if (card[i] == 39) {
				stack.push_back(8);
			}
			else if (card[i] == 41) {
				stack.push_back(10);
			}
			else if (card[i] == 43) {
				stack.push_back(12);
			}
		}

		std::cout << "\n";

		for (int i = 0; i < static_cast<int>(stack.size()); i++) {
			char c[3];
			c[0] = dcm.at(stack.at(i));
			c[1] = dcm.at(stack.at(i) + 1);
			c[2] = '\0';
			std::cout << c << "  ";
		}
		std::cout << "\n";
	}


	//get std::string
	std::string getstr(const char *name, int intdata) {
		std::string str = name, temp;
		std::stringstream ss;
		ss << intdata;
		ss >> temp;
		str += temp;

		return str;
	}


	//wait read json[done]
	void wait(clientsocket &sock) {
		while (true) {
			json r;
			sock.read(r);
			if (r.is_null())
				continue;
			if (r.find("done") != r.end())
				break;
		}
	}


	//int to json
	json iarrtojson(int *data, int datalen, const char *main_json_name, const char *second_json_name) {
		return iarrtojson(data, datalen, main_json_name, second_json_name, false, -1);
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


	//return playernumber
	int getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname) {
		return getjsonvalue(tj, buf, getlen, getplayernumber, tjname, "card");
	}

	int getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname, const char *getname) {
		json j = tj[tjname];
		int playernumber = -1;

		if (getplayernumber && j.find("playernumber") != j.end()) { //要拿使用者編號 與 真的有傳使用者編號 than
			playernumber = j["playernumber"];
		}

		for (int i = 0; i < getlen; i++) {
			buf[i] = j[getstr(getname, i + 1)];
		}

		return playernumber;
	}


	//int to std::string to json
	void sendcardtoserver(clientsocket &sock ,int *data,int datalen,const char *name) {
		std::string str;
		std::stringstream ss;
		json j;
		//int[] to string
		for (int i = 0; i < 7; i++) {
			std::string temps;
			ss << data[i];
			ss >> temps;
			str.push_back(temps.at(0));
			str.push_back(',');
			ss.str("");
			ss.clear();
		}
		j[name] = str;

		sock.send(j);
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
	void sendpolling(clientsocket &sock,int outcard,int outplayer) {
		
		//處理胡牌
		int whocard[17];
		cpycard(card, whocard, 17);
		whocard[cptr] = outcard;
		if (card_check(whocard, 17)) {
			std::cout << "你胡到別人牌了!\n";
			json j, secj;
			for (int i = 0; i < 17; i++) {
				if (i == cptr)
					continue;

				secj[getstr("card", i + 1 - (i%cptr))] = whocard[i];
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


	//Player Start

	void start() {
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
	}


	void print(int *data, int len) {
		for (int i = 0; i < len; i++) {
			std::cout << data[i] << " ";
		}
		std::cout << "\n";
	}


	//看缺什麼牌
	void get_need_card(int *tempcard,int len) {
		int tcard[17];
		cpycard(tempcard, tcard, len);
		card_sort(tcard, len, true); //先排序-小到大
		need_card.clear();
		need_card_index.clear();

		//這邊不討論孤張，因為在上一層的function已經判斷過了
		for (int i = 0; i < len - 1; i++) {
			int temp = 0;
			if (!card_level(tcard[i], tcard[i + 1])) { //如果不在同個量級的話 than
				continue;
			}
			if (tcard[i + 1] - tcard[i] >= 3) { //如果無法成順組 than
				continue;
			}

			if (tcard[i] == tcard[i + 1]) { //如果是2張對組的話 than
				need_card.push_back(tcard[i]); //push實際數字
				
			}
			else { //不是對組那就是順組了，如果是順組的話 than
				if (tcard[i + 1] - tcard[i] == 1) { //如果是 34 這類的牌型 than
					if (tcard[i] % 10 == 1) {
						need_card.push_back(tcard[i + 1] + 1);
					}
					else if (tcard[i] % 10 == 8) {
						need_card.push_back(tcard[i] - 1);
					}
					else {
						need_card.push_back(tcard[i] - 1);
						need_card.push_back(tcard[i + 1] + 1);
						//push index
						need_card_index.push_back(i);
						need_card_index.push_back(i + 1);
					}
				}
				else { //如果是 35這類的牌型 than
					need_card.push_back(tcard[i] + 1);
				}
			}
			//push index
			need_card_index.push_back(i); 
			need_card_index.push_back(i + 1);

		}
		//print
		/*for (int i = 0; i < static_cast<int>(need_card.size()); i++)
			std::cout << need_card.at(i) << " ";*/
	}


	void set_mycardtimes(int *card,int len) {
		for (int i = 0; i < 17; i++) {
			if (card[i] == -1)
				continue;
			card_times[card[i]]--;
		}
	}

	void set_mycardtimes(int card) {
		if (!card)
			return;
		card_times[card]--;
	}


	//拿出已完成的順對組,return 改變後的len的值
	int card_organize(int *card,int len) {
		std::vector<int> buf[4];
		int expect[4];
		expect[0] = card_organize_down(card, len, buf[0], true, true);
		expect[1] = card_organize_down(card, len, buf[1], true, false);
		expect[2] = card_organize_down(card, len, buf[2], false, true);
		expect[3] = card_organize_down(card, len, buf[3], false, false);

		int max = 0;
		for (int i = 0; i < 4; i++) {
			if (max < expect[i]) {
				max = i;
			}
		}
		//test print
		/*cout << "buf[max]: ";
		for (int i = 1; i < static_cast<int>(buf[max].size()); i++) {
			cout << buf[max].at(i) << " ";
			if (i == buf[max].at(0))
				cout << "_";
		}
		cout << "\n";*/

		for (int i = 1; i < static_cast<int>(buf[max].size()); i++) {
			card[i - 1] = buf[max].at(i);
		}

		return buf[max].at(0);
	}


	//card_organize的子函式，用來拿出順and對組到後面，return 總期望值
	int card_organize_down(int *card,int len,std::vector<int> &buf,bool smallorder,bool preorder) {
		int m, buf_order[17], buf_pair[17], optr = 0, pptr = 0, _buf[17], bptr = 0;
		//clean
		for (int i = 0; i < 17; i++) {
			buf_order[i] = -1;
			buf_pair[i] = -1;
			_buf[i] = -1;
		}
		//先排序一遍
		card_sort(card, len, smallorder);
		//set m  依照排列順序,m要跟著變換
		if (smallorder) {
			m = 1;
		}
		else {
			m = -1;
		}
		//決定先取順組還是先取對組
		if (preorder) {
			optr = card_check_order(card, len, buf_order, m);
			pptr = card_check_pair(card, len, buf_order, optr, buf_pair);
		}
		else {
			pptr = card_check_pair(card, len, buf_pair);
			optr = card_check_order(card, len, buf_pair, pptr, buf_order, m);
		}


		buf.push_back(len - optr - pptr); //分隔點
		//要找出最後那2張當眼的，並記錄到buf[]裡
		for (int i = 0; i < len; i++) {
			if (cmp(buf_order, optr, 0, i) == -1 && cmp(buf_pair, pptr, 0, i) == -1) {
				buf.push_back(card[i]);
				_buf[bptr] = card[i];
				bptr++;
			}
		}
		for (int i = 0; i < optr; i++) {
			buf.push_back(card[buf_order[i]]);
		}
		for (int i = 0; i < pptr; i++) {
			buf.push_back(card[buf_pair[i]]);
		}

		return get_card_expect(_buf,bptr);
	}


#pragma region 確認是否胡牌
	//card_check第三層，找出順子，並回傳是否有此順組，沒有則-1，有則>1
	//p為開始的index，m為指定要找的昰遞增或遞減
	int order_check(int *card, int *order, int len, int orderlen, int *buf, int bptr, int start, int m) {
		//new-用期望、預期他會出現來做。ex:input 1 預期2、3 然後尋找有沒有相對應的2、3存在，有的話就是順子，提取出來
		//2018/3/9-01:30 改了3天改完了Q_Q，重寫了10遍，換了1次主要想法
		if (cmp(order, orderlen, 0, start) == -1)
			return -1;
		buf[bptr] = start;
		bptr++;
		int ans[2] = { card[start] + m,card[start] + m + m };
		for (int i = 0; i < 2; i++) {
			//如果要找的index等於超過array長度，則跳出，因為已違規或是檢查到array最後一個了，沒必要檢查
			if (start >= len - 1)
				return -1;
			//如果index已經找過了，就跳過
			while (true) {
				start = cmp(card, len, start + 1, ans[i]);
				if (start == -1)
					break;
				if (cmp(order, orderlen, 0, start) == -1)
					continue;
				int temp = cmp(buf, bptr, 0, start);
				if (temp == -1) {
					break;
				}
			}

			//如果==-1，那就代表沒有此預期的順組，所以回傳-1
			if (start != -1) {
				buf[bptr] = start;
				bptr++;
			}
			else {
				return -1;
			}
		}
		return bptr;
	}
	//card_check第三層，找出對組，並回傳出現幾次
	//m為指定數字，bptr為*buf當前要記錄的開始位置
	int pair_appear(int *card, int *pair, int len, int pairlen, int *buf, int bptr, int start) {
		/*if (cmp(pair, pairlen, 0, start) == -1)
			return -1;
		buf[bptr] = start;
		bptr++;
		int ans=card[start];
		for (int i = 0; i < 2; i++) {
			//如果要找的index等於超過array長度，則跳出，因為已違規或是檢查到array最後一個了，沒必要檢查
			if (start >= len - 1)
				return -1;
			//如果index已經找過了，就跳過
			while (true) {
				start = cmp(card, len, start + 1, ans);
				if (start == -1)
					break;
				if (cmp(pair, pairlen, 0, start) == -1)
					continue;
				int temp = cmp(buf, bptr, 0, start); //確認是否在buf出現過
				if (temp == -1) { //沒出現，跳出迴圈
					break;
				}
			}

			//如果==-1，那就代表沒有此預期的順組，所以回傳-1
			if (start != -1) {
				buf[bptr] = start;
				bptr++;
			}
			else {
				return -1;
			}
		}
		return bptr;*/

		return this->order_check(card, pair, len, pairlen, buf, bptr, start, 0);
	}

	//card_check第二層，順組的檢查
	int card_check_order(int *card, int cardlen,int *buf,int m) {
		int pair[17];
		for (int i = 0; i < 17; i++) {
			pair[i] = -1;
		}
			
		return this->card_check_order(card, cardlen, pair, 17, buf,m);
	}
	int card_check_order(int *card,int cardlen,int *pair,int pairlen,int *buf,int m) {
		int order[17], orderlen = 0, bptr = 0;
		//clean and set
		for (int i = 0; i < 17; i++) {
			buf[i] = -1; //clean buf
			if (cmp(pair, pairlen, 0, i) == -1) { //加入0~16，但如果已經在pair裡的數字，將不會加進order裡
				order[orderlen] = i;
				orderlen++;
			}
		}
		
		//order check start
		for (int i = 0; i < cardlen-2; i++)
		{
			bool outst = false;
			//如果要檢查的值已經被跳過或檢查過則不執行
			for (int j = 0; j <= bptr; j++) {
				if (i == buf[j] || bptr >= cardlen) {
					outst = true;
					break;
				}
			}
			if (outst == true) {
				continue;
			}
			
			int temp = order_check(card, order, cardlen, orderlen, buf, bptr, i, m);
			if (temp != -1)
				bptr = temp; 
		}
		//清buf不是順組index的資料，因為實作方面會先記錄，可能有不在bptr範圍裡但有不是-1的值的
		for (int i = bptr; i < 17; i++) {
			buf[i] = -1;
		}
		//print order[]
		/*cout << "order buf:";
		for (int i = 0; i < orderlen; i++)
		cout << buf[i] << " ";
		cout << endl;*/

		return bptr;
	}


	//card_check第二層，對組的檢查
	int card_check_pair(int *card, int cardlen,int *buf) {
		int order[17];
		for (int i = 0; i < 17; i++) {
			order[i] = -1;
		}
		return this->card_check_pair(card, cardlen, order, 17,buf);
	}
	int card_check_pair(int *card, int cardlen, int *order, int orderlen, int *buf) {
		return this->card_check_order(card, cardlen, order, orderlen, buf, 0);
		
		/*int pair[17], pairlen = 0, bptr = 0;
		//clean and set
		for (int i = 0; i < 17; i++) {
			buf[i] = -1; //clean buf
			if (cmp(order, orderlen, 0, i) == -1) { //加入0~16，但如果已經在pair裡的數字，將不會加進order裡
				pair[pairlen] = i;
				pairlen++;
			}
		}

		//order check start
		for (int i = 0; i < cardlen - 2; i++)
		{
			bool outst = false;
			//如果要檢查的值已經被跳過或檢查過則不執行
			for (int j = 0; j <= bptr; j++) {
				if (i == buf[j] || bptr >= cardlen) {
					outst = true;
					break;
				}
			}
			if (outst == true) {
				continue;
			}

			int temp = pair_appear(card, pair, cardlen, pairlen, buf, bptr, i);
			if (temp != -1)
				bptr = temp;
		}
		//清buf不是對組index的資料，因為實作方面會先記錄，可能有不在bptr範圍裡但有不是-1的值的
		for (int i = bptr; i < 17; i++) {
			buf[i] = -1;
		}
		//print pair[]
		cout << "pair buf:";
		for (int i = 0; i < pairlen; i++)
		cout << buf[i] << " ";
		cout << endl;

		return bptr;*/
	}
	//看有沒有等於m值的，有的話回傳他的位置，沒有則回傳-1
	//m為指定數字
	int cmp(int *card, int len, int start, int m) {
		if (m == -1)
			return -1;
		for (int i = start; i < len; i++) {
			if (card[i] == m) {
				return i;
			}
		}
		return -1;
	}

	//檢查牌是否為胡牌牌型 1=胡牌 0=詐胡
	//第一層，統一管理
	//order[] and pair[] 都是記錄card[]的index資訊，不是記錄實際的值
	int card_check_check(int *card, int len,bool smallorder,bool preorder) {
		int m,buf_order[17], buf_pair[17], optr = 0, pptr = 0, buf[17], bptr = 0;
		//clean
		for (int i = 0; i < 17; i++) {
			buf_order[i] = -1;
			buf_pair[i] = -1;
			buf[i] = -1;
		}
		//先排序一遍
		card_sort(card, len, smallorder);
		//set m  依照排列順序,m要跟著變換
		if (smallorder) {
			m = 1;
		}else {
			m = -1;
		}
		//決定先取順組還是先取對組
		if (preorder) {
			optr = card_check_order(card, len, buf_order, m);
			pptr = card_check_pair(card, len, buf_order, optr, buf_pair);
		}else {
			pptr = card_check_pair(card, len, buf_pair);
			optr = card_check_order(card, len, buf_pair, pptr, buf_order,m);
		}
		
		
		
		//要找出最後那2張當眼的，並記錄到buf[]裡
		for (int i = 0; i < len; i++) {
			if (cmp(buf_order, optr, 0, i) == -1 && cmp(buf_pair, pptr, 0, i) == -1) {
				buf[bptr] = i;
				bptr++;
			}
		}

		//print all buf[]
		/*for (int i = 0; i < optr; i++) {
			std::cout << buf_order[i] << " ";
		}
		std::cout << std::endl;

		for (int i = 0; i < pptr; i++) {
			std::cout << buf_pair[i] << " ";
		}
		std::cout << std::endl;

		
		
		for (int i = 0; i < bptr; i++)
			std::cout << buf[i] << " ";
		std::cout << std::endl;
		std::cout << "bptr:" << bptr << std::endl;*/

		//看是不是真的最後2張當眼的是一樣的牌
		if (bptr < 2 || bptr > 2 || card[buf[0]] != card[buf[1]]) {
			return -1;
		}
		
		
		return 1;
	}


	//第0層
	//因為檢查胡牌要判斷4種情況，所以在這統一處理
	//胡牌return 1   沒胡牌return 0
	int card_check(int *tempcard, int len) {
		int card[17];
		cpycard(tempcard, card, len);
		
		if (card_check_check(card, len, false, false) == 1)
			return 1;
		if (card_check_check(card, len, true, false) == 1)
			return 1;
		if (card_check_check(card, len, false, true) == 1)
			return 1;
		if (card_check_check(card, len, true, true) == 1)
			return 1;

			return 0;
	}

#pragma endregion</span>

	//cardsort
	void card_sort(int *card, int len, bool SmalltoBig) {
		std::sort(card, card + len);
		if (!SmalltoBig)
			std::reverse(card, card + len);
	}


#pragma region 棄牌系統-估價函式
	//估價函式,return index , can throw;
	int card_price(int *card, int len) {
		//這邊不能複製card array  如果用副本來做sort後，回傳的index會是錯誤的
		//先sort一遍
		card_sort(card, len, true);

		//0.-先丟單獨的字牌
		for (int i = 0; i < len; i++) {
			if (card[i] >= 31) { //如果是字牌
				if (getcardtimes(card, len, card[i]) >= 2) { //如果>=2張
					continue;
				}
				else {
					return i;
				}
			}
		}

		//1.-再丟孤張
		for (int i = 0; i < len; i++) {
			if (card[i] < 30) { //如果不是字牌 than
				if (card_expect(card, len, i) == false) //如果沒有可以等的順組 or 對組 than
					return i;
			}
		}

		//2.-真正的估價函式-期望值
		int index = card_expect(card, len, card_times);
		if (index != -1) {
			return index;
		}

		/*要補作丟別人丟過的牌*/

		return 0;

	}
#pragma endregion</span>

	//傳回總期望值
	int get_card_expect(int *tempcard,int len) {
		std::vector<int> calculated, expect;
		int card[17];
		cpycard(tempcard, card, len);

		card_sort(card, len, true); //先排序-小到大

		//這邊不討論孤張，因為在上一層的function已經判斷過了
		for (int i = 0; i < len - 1; i++) {
			int temp = 0;
			if (!card_level(card[i], card[i + 1])) { //如果不在同個量級的話 than
				continue;
			}
			if (card[i + 1] - card[i] >= 3) {
				continue;
			}
			if (card[i] <= 0) {
				continue;
			}

			if (card[i] == card[i + 1]) { //如果是2張對組的話 than
				if (search_vector(calculated, card[i]))
					continue;
				expect.push_back(card_times[card[i]]);
				calculated.push_back(card[i]);
			}
			else { //不是對組那就是順組了，如果是順組的話 than
				if (card[i] > 30)
					continue;
				if (card[i + 1] - card[i] == 1) { //如果是 34 這類的牌型 than
					if (card[i] % 10 == 1) {
						if (search_vector(calculated, card[i + 1] + 1))
							continue;
						expect.push_back(card_times[card[i + 1] + 1]);
						calculated.push_back(card[i + 1] + 1);

					}
					else if (card[i] % 10 == 8) {
						if (search_vector(calculated, card[i] - 1))
							continue;
						expect.push_back(card_times[card[i] - 1]);
						calculated.push_back(card[i] - 1);

					}
					else {
						if (!search_vector(calculated, card[i] - 1)) {
							temp += card_times[card[i] - 1];
							calculated.push_back(card[i] - 1);
						}
						if (!search_vector(calculated, card[i + 1] + 1)) {
							temp += card_times[card[i + 1] + 1];
							calculated.push_back(card[i + 1] + 1);
						}
						expect.push_back(temp);
					}
				}
				else { //如果是 35這類的牌型 than
					if (search_vector(calculated, card[i] + 1))
						continue;
					expect.push_back(card_times[card[i] + 1]);
					calculated.push_back(card[i] + 1);
				}
			}
			
		}

		int temp = 0;
		for (int i = 0; i < static_cast<int>(expect.size()); i++) {
			temp += expect.at(i);
		}
		
		return temp;
	}


	//算出期望值以便丟牌,return 要丟的牌的index
	int card_expect(int *card, int len, int *cardtimes) {
		std::vector<int> buf, calculated, expect;
		int times[44]; //看出現次數，來讓出現次數少的被丟出去，心中要有個graph
		for (int i = 0; i < 44; i++)
			times[i] = 0;

		card_sort(card, len, true); //先排序-小到大

		//這邊不討論孤張，因為在上一層的function已經判斷過了
		for (int i = 0; i < len - 1; i++) {
			int temp = 0;
			if (!card_level(card[i], card[i + 1])) { //如果不在同個量級的話 than
				expect.push_back(99);
				continue;
			}
			if (card[i + 1] - card[i] >= 3) {
				expect.push_back(99);
				times[card[i]]++;
				times[card[i + 1]]++;
				continue;
			}
			

			if (card[i] == card[i + 1]) { //如果是2張對組的話 than
				expect.push_back(cardtimes[card[i]]);
			}
			else { //不是對組那就是順組了，如果是順組的話 than
				if (card[i + 1] - card[i] == 1) { //如果是 34 這類的牌型 than
					if (card[i] % 10 == 1) {
						expect.push_back(cardtimes[card[i + 1] + 1]);
					}
					else if (card[i] % 10 == 8) {
						expect.push_back(cardtimes[card[i] - 1]);
					}
					else {
						temp += cardtimes[card[i] - 1];
						temp += cardtimes[card[i + 1] + 1];
						expect.push_back(temp);
					}
				}
				else { //如果是 35這類的牌型 than
					expect.push_back(cardtimes[card[i] + 1]);
				}
			}
			//出現次數++
			times[card[i]]++;
			times[card[i+1]]++;
		}

		int minexpect = search_vextor_min(expect); //先取得最小期望值的那個對or順組
		int min = search_array_min(times, len); //取得出現次數小的
		//丟期望值低+連結次數少
		for (int i = 0; i < static_cast<int>(expect.size()); i++) {
			//如果期望值最低，而且出現次數也最小，就return index
			if (minexpect == -1)
				break;

			if (times[card[minexpect]] == min)
				return minexpect;
			if (times[card[minexpect + 1]] == min)
				return minexpect + 1;

			minexpect = search_vector_index(expect, minexpect + 1, expect.at(minexpect)); //下一個較低期望值的index
		}
			
		//丟期望值低的
		minexpect = search_vextor_min(expect); //重新取得最小期望值的對組or順組
		//丟出較靠1&9的數字
		if (abs(5 - card[minexpect]) > abs(5 - card[minexpect + 1])) //ex:abs(5-1)>abs(5-2)
			return minexpect;
		else if (abs(5 - card[minexpect]) < abs(5 - card[minexpect + 1]))
			return minexpect + 1;
		return -1;
	}


	//回傳*data裡的最小值
	int search_array_min(int *data,int len) {
		int min = 1;
		for (int i = 0; i < len; i++) {
			if (!data[i])
				continue;
			if (min > data[i])
				min = i;
		}
		return min;
	}


	//找buf最小值 return index
	int search_vextor_min(std::vector<int> &buf) {
		int min = 0;
		for (int i = 0; i < static_cast<int>(buf.size()); i++) {
			if (buf.at(min) > buf.at(i))
				min = i;
		}
		return min;
	}


	//找assgin的值有沒有在buf裡出現過
	bool search_vector(std::vector<int> &buf, int assign) {
		for (int i = 0; i < static_cast<int>(buf.size()); i++) {
			if (buf.at(i) == assign)
				return true;
		}
		return false;
	}


	//找出vector buf裡的指定值,return index
	int search_vector_index(std::vector<int> &buf, int start, int assign) {
		for (int i = start; i < static_cast<int>(buf.size()); i++) {
			if (buf.at(i) == assign)
				return i;
		}
		return -1;
	}


	//複製array  *card是複製可以用的
	void cpycard(int *tempcard, int *card,int len) {
		for (int i = 0; i < 17; i++)
			card[i] = -1;
		for (int i = 0; i < len; i++)
			card[i] = tempcard[i];
	}

	//這要幹嘛??
	void card_grouping(int *card, int len, bool smallorder, bool preorder, std::vector<int> &buf) {
		int m, buf_order[17], buf_pair[17], optr = 0, pptr = 0;
		//clean
		for (int i = 0; i < 17; i++) {
			buf_order[i] = -1;
			buf_pair[i] = -1;
		}
		//先排序一遍
		card_sort(card, len, smallorder);
		//set m  依照排列順序,m要跟著變換
		if (smallorder) {
			m = 1;
		}
		else {
			m = -1;
		}
		//決定先取順組還是先取對組
		if (preorder) {
			optr = card_check_order(card, len, buf_order, m);
			pptr = card_check_pair(card, len, buf_order, optr, buf_pair);
		}
		else {
			pptr = card_check_pair(card, len, buf_pair);
			optr = card_check_order(card, len, buf_pair, pptr, buf_order, m);
		}

		//要找出最後那2張當眼的，並記錄到buf[]裡
		for (int i = 0; i < len; i++) {
			if (cmp(buf_order, optr, 0, i) == -1 && cmp(buf_pair, pptr, 0, i) == -1) {
				buf.push_back(i);
			}
		}
	}


	//這張牌，有沒有組成可以等待的順or對 ex:input 5  search for 3 4 5 6 7
	//cptr為指定的index
	bool card_expect(int *card,int len,int cptr) {
		std::vector<int> buf;
		if (card[cptr] % 10 == 1) {
			buf.push_back(card[cptr] + 1);
			buf.push_back(card[cptr] + 2);

		}else if (card[cptr] % 10 == 9) {
			buf.push_back(card[cptr] - 2);
			buf.push_back(card[cptr] - 1);

		}else if (card[cptr] % 10 == 2) {
			buf.push_back(card[cptr] - 1);
			buf.push_back(card[cptr] + 1);
			buf.push_back(card[cptr] + 2);

		}else if (card[cptr] % 10 == 8) {
			buf.push_back(card[cptr] - 2);
			buf.push_back(card[cptr] - 1);
			buf.push_back(card[cptr] + 1);

		}else {
			buf.push_back(card[cptr] - 2);
			buf.push_back(card[cptr] - 1);
			buf.push_back(card[cptr] + 1);
			buf.push_back(card[cptr] + 2);
		}
		//看有沒有可以等的順組
		for (int i = 0; i < static_cast<int>(buf.size()); i++) {
			if (cmp(card, len, 0, buf.at(i)) != -1)
				return true;
		}
		//看有沒有可以等的對組
		if (getcardtimes(card, len, card[cptr]) >= 2)
			return true;

		return false;
	}


	//判斷這兩張牌是不是在同個量級
	//yes return true     not return false
	bool card_level(int card1, int card2) {
		if (card1 < 10 && card1>0 && card2 < 10 && card2>0) { //萬字
			return true;
		}
		if (card1 < 20 && card1>10 && card2 < 20 && card2>10) { //條子
			return true;
		}
		if (card1 < 30 && card1>20 && card2 < 30 && card2>20) { //筒子
			return true;
		}

		return false;
	}


	//取得指定數字m牌裡出現幾次，
	int getcardtimes(int *card,int len,int m) {
		int count = 0;
		for (int i = 0; i < len; i++) {
			if (card[i] == m) {
				count++;
			}
		}
		return count;
	}

};
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