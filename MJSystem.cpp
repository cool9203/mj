//牌局系統-MJSystem
//沒有起始的骰子概念、也沒有時間的概念
#ifndef MJSYSTEM_CPP
#define MJSYSTEM_CPP



#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include "myserversocket.h"
#include "clientsocket.h"

using namespace std;
namespace MJ{
	class MJSystem {
	private:
		int card_order[136] = { 0 };
		int card_ptr = 0;
		int out_card_order[136] = { 0 };
		int out_card_order_ptr = 0;
		int main_player = 0;
		//下面3個變數，如果走輪詢式問吃碰胡牌，那這3個變數就無用了Q_Q，但還是先寫起來，要安全一定是系統這邊要控管所有的牌，包括玩家的牌也是。
		int card_player1[17], card_player2[17], card_player3[17], card_player4[17];
		int player1_card_ptr = 0, player2_card_ptr = 0, player3_card_ptr = 0, player4_card_ptr = 0;
		int out_card_player1[16], out_card_player2[16], out_card_player3[16], out_card_player4[16];
		//給定玩家編號與控管玩家是否進場了
		int player_queue[4] = { 0,0,0,0 };



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
		int card_check_order(int *card, int cardlen, int *buf, int m) {
			int pair[17];
			for (int i = 0; i < 17; i++) {
				pair[i] = -1;
			}

			return this->card_check_order(card, cardlen, pair, 17, buf, m);
		}
		int card_check_order(int *card, int cardlen, int *pair, int pairlen, int *buf, int m) {
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
		int card_check_pair(int *card, int cardlen, int *buf) {
			int order[17];
			for (int i = 0; i < 17; i++) {
				order[i] = -1;
			}
			return this->card_check_pair(card, cardlen, order, 17, buf);
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
		int card_check_check(int *card, int len, bool smallorder, bool preorder) {
			int m, buf_order[17], buf_pair[17], optr = 0, pptr = 0, buf[17], bptr = 0;
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
			sort(card, card + len);
			if (!SmalltoBig)
				reverse(card, card + len);
		}


		//複製array  *card是複製可以用的
		void cpycard(int *tempcard, int *card, int len) {
			for (int i = 0; i < 17; i++)
				card[i] = -1;
			for (int i = 0; i < len; i++)
				card[i] = tempcard[i];
		}
	
	
		//放牌
		void card_push(int start, int end, int p) {
			for (int i = start; i <= end; i++) {
				for (int j = 0; j < 4; j++) {
					card_order[p] = i;
					p++;//類似指標的變數。指著陣列的位置
				}
			}
		}
	
	
		//交換2數
		void swap(int *a, int *b) {
			int temp = *a;
			*a = *b;
			*b = temp;
		}


		//------------------------public---------------------------
	public:
	
		//洗牌
		void card_rand() {
			card_ptr = 0;
			//先放牌Q_Q
			card_push(1, 9, 0 * 4); //萬
			card_push(11, 19, 9 * 4); //條
			card_push(21, 29, 18 * 4); //筒
									   //為了讓字牌判斷好點，所以字牌都多間隔1
			for (int i = 31; i <= 43; i = i + 2) {
				card_push(i, i, (27 + (i - 31) / 2) * 4);
			}
			//再洗牌Q_Q
			random_device rd; //利用硬體系統給的值來取值
			default_random_engine generator(rd()); //初始化，並利用rd()的值找出相對應的種子碼??
			uniform_int_distribution<int> distribution(0, 135); //設定範圍與決定要產生什麼類型的亂數 ex:int float..

			for (int i = 0; i < 136; i++) {
				swap(&card_order[i], &card_order[distribution(generator)]); //呼叫亂數則使用上面2個的變數來呼叫、使用
			}
		}


		//取得玩家編號
		int GetPlayernumber() {
			for (int i = 0; i < 4; i++) {
				if (player_queue[i] == 0) {
					player_queue[i] = i + 1;
					return player_queue[i];
				}
			}
			return -1;
		}


		//新的一局-newgame
		void Newgame() {
			//先清空所有應該清空的變數
			for (int i = 0; i < 17; i++) {
				card_player1[i] = 0;
				card_player2[i] = 0;
				card_player3[i] = 0;
				card_player4[i] = 0;
			}
			for (int i = 0; i < 16; i++) {
				out_card_player1[i] = 0;
				out_card_player2[i] = 0;
				out_card_player3[i] = 0;
				out_card_player4[i] = 0;
			}
			for (int i = 0; i < 136; i++) {
				out_card_order[i] = 0;
			}
			card_ptr = 0;
			out_card_order_ptr = 0;
			player1_card_ptr = 0;
			player2_card_ptr = 0;
			player3_card_ptr = 0;
			player4_card_ptr = 0;
			//再來洗牌
			card_rand();
		}


		//玩家拿牌
		int Getcard() {
			if (card_ptr == 122)
				return -1;
			card_ptr++;
			return card_order[card_ptr-1];
		}


		//檢查此吃碰牌是否合法，要傳牌的index，合法回傳1，不合法回傳0，吃st=false，碰st=true
		int Sendcard(int card1, int card2,int getcard,bool st) {
			int card[3] = { card1,card2,getcard };
			sort(card, card + 3);
			if (st == false) { //false 是順組==吃
				if (card[0] + 1 != card[1] || card[1] + 1 != card[2])
					return 0;
			}else if (st == true) { //true 是對組==碰
				if (card1 != card2 || card1 != getcard)
					return 0;
			}
			return 1;
		}


		//檢查此槓牌是否合法，合法回傳1，不合法回傳0
		int Sendcard(int card0, int card1, int card2, int getcard) {
			if (card0 != card1 || card0 != card2 || card0 != getcard) {
				return 0;
			}
			return 1;
		}


		//清除vector<> client的動態配置記憶體
		void endclient(std::vector<clientsocket*> &client) {
			for (int i = 0; i < static_cast<int>(client.size()); i++) {
				delete client[i];
			}
			client.clear();
		}


		//等待client.at(assign) 傳回 json[done]
		void wait(std::vector<clientsocket*> &client,int assign) {
			while (true) {
				json r;
				client.at(assign)->read(r);
				if (r.is_null())
					continue;
				if (r.find("done") != r.end())
					break;
			}
		}


		//get std::string(name+intdata)
		std::string getstr(const char *name, int intdata) {
			std::string str = name, temp;
			std::stringstream ss;
			ss << intdata;
			ss >> temp;
			str += temp;

			return str;
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


		/*
		label	return
		[no]	0
		[eat]	1
		[pung]	2
		[gun]	3
		[who]	4
		else	-1
		*/
		int getjsonmessage_polling(json &j) {
			if (j.size() == 0)
				return -1;
			if (j.find("no") != j.end())
				return 0;
			else if (j.find("eat") != j.end())
				return 1;
			else if (j.find("pung") != j.end())
				return 2;
			else if (j.find("gun") != j.end())
				return 3;
			else if (j.find("who") != j.end())
				return 4;
			else
				return -1;
		}


		//傳給其他玩家此次吃碰槓胡的資訊
		//要先改過main_player才能傳資訊
		void sendtoother(std::vector<clientsocket*> &client, json j) {
			for (int i = 0; i < 4; i++) {
				client.at(i)->send(j);
				wait(client, i);
			}
		}

		void sendtoother(std::vector<clientsocket*> &client, int *data, int datalen, const char *main_json_name, const char *second_json_name) {
			json j = iarrtojson(data, datalen, main_json_name, second_json_name, true, main_player);
			sendtoother(client, j);

		}
		

		void start() {
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
			myserversocket *server = new myserversocket(socketfd, 9990, 10, "127.0.0.1");
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
			onegame(server);
			server->close();
		}

		
		void onegame(myserversocket *&server) {
			//create client vector
			std::vector<clientsocket*> client;
			bool who = false;
			int whocard[17];

			//get client into vector and send playernumber
			for (int i = 0; i < 4; i++) {
				clientsocket *c = server->accept();
				if (!c->status()) {
					delete c;
					continue;
				}
				json j;
				j["youplayernumber"] = i;
				c->send(j);
				client.push_back(c);
				wait(client, i);
			}

			//send json["gamestart"]
			for (int i = 0; i < static_cast<int>(client.size()); i++) {
				json j;
				j["gamestart"];
				client.at(i)->send(j);
				wait(client, i);
			}

			//card random
			card_rand();

			//send 4*4card to player
			for (int i = 0; i < 4; i++) {
				for (int player = 0; player < static_cast<int>(client.size()); player++) {
					for (int count = 0; count < 4; count++) {
						json j;
						j["sendcard"] = Getcard();
						client.at(player)->send(j);
						wait(client, player);
					}
				}
			}

			//通知玩家回合&讓玩家抽牌 -> 玩家丟牌 -> 看其他玩家是否要動作 -> if(胡牌) than {break} else {contiune}  -> loop
			while (true){
				//流局了
				if (card_ptr == 122) {
					std::cout << "endgame\n";
					json endj;
					endj["endgame"];
					sendtoother(client, endj);
					break;
				}
				//send to mainplayer(莊家) one card and notice he turn.  and get mainplayer's outcard or he self who.
				int outcard = -1;
				for (int i = 0; i < 1; i++) {
					json j;
					//sned one card and notice he turn.
					j["youturn"];
					j["sendcard"] = Getcard();
					client.at(main_player)->send(j);
					wait(client, main_player);

					//get outcard or client he self who(自摸).
					j.clear();
					while (true) {
						client.at(main_player)->read(j);
						if (j.is_null())
							continue;
						//std::cout << "server get  :" << j << "\n\n";
						json d;
						d["done"];
						client.at(main_player)->send(d);
						break;
					}

					for (json::iterator it = j.begin(); it != j.end(); ++it) {
						if (it.key() == "outcard") { //如果是丟牌
							outcard = it.value();
							break;

						}else if (it.key() == "who") { //如果他自摸
							getjsonvalue(j, whocard, 17, false, "who");
							
							if (card_check(whocard, 17) != 0) {
								//std::cout << "server get who.\n";
								who = true;
							}
							break;
						}else if (it.key() == "gun") { //如果他要槓牌
							int tempcard[4]; 

							getjsonvalue(j, tempcard, 4, false, "gun");
							if (Sendcard(tempcard[0], tempcard[1], tempcard[2], tempcard[3]) == 1) {
								//處理槓牌
							}
						}
					}
				}

				//one check he self who
				if (who == true) { //自摸成立
					break;
				}


				while (true) { //連續吃碰槓while
					bool player_do = false;
					//先傳main_player丟的牌給其他3位玩家
					for (int i = 0; i < 4; i++) {
						if (i == main_player)
							continue;

						json j, secj;
						secj["playernumber"] = main_player;
						secj[getstr("card", 1)] = outcard;
						j["outcard"] = secj;
						client.at(i)->send(j);
						wait(client, i);

					}
					//輪詢-問各個使用者是否需要作事(吃碰槓胡)
					int deat = -1, dpung = -1, dgun = -1, dwho = -1;
					vector<json> r;
					for (int i = 0; i < 4; i++) {
						json tempjs;
						r.push_back(tempjs);
					}

					//先問各個玩家想執行的動作,並用vector client[index]紀錄下來
					for (int i = 1; i < 4; i++) {
						int number = (main_player + i) % 4;
						json j, d;
						j["do"];
						d["done"];
						//傳 [do] 讓他了解現在是問他
						client.at(number)->send(j);
						wait(client, number);

						//收執行動作的資訊
						while (true) {
							client.at(number)->read(r.at(number)); //read
							if (r[number].is_null())
								continue;

							client.at(number)->send(d); //sned [done]
							break;
						}

						//處理收到的資訊,來看他是想做什麼事
						int thing = getjsonmessage_polling(r[number]);
						switch (thing) {
						case 0: //no
							break;
						case 1: //eat
							if (main_player != (number - 1) % 4) //這邊還要多傳出1個訊息給client,讓他知道他傳錯了 or ...
								break;
							deat = number;
							break;
						case 2: //pung
							if (dpung == -1)
								dpung = number;
							break;
						case 3: //gun
							if (dgun == -1)
								dgun = number;
							break;
						case 4: //who
							if (dwho == -1)
								dwho = number;
							break;
						}
					}

					//找完吃碰槓胡的順序後，決定下個main_player和檢查client傳過來的資訊，來確定是否是正確的動作
					if (dwho != -1) { //有人胡牌
						getjsonvalue(r[dwho], whocard, 16, false, "who");
						whocard[16] = outcard;
						if (card_check(whocard, 17) != 0) {
							main_player = dwho;
							who = true;
							std::cout << "player[" << main_player << "]:who\n\n";
							break;
						}

					}
					else if (dpung != -1 || dgun != -1) { //碰槓牌
						int tempcard[4];
						if (dpung != -1) {
							getjsonvalue(r[dpung], tempcard, 2, false, "pung");
							std::cout << "pungcard:" << tempcard[0] << " " << tempcard[1] << " " << outcard << std::endl;
							if (Sendcard(tempcard[0], tempcard[1], outcard, true) == 1) {
								main_player = dpung;
								player_do = true;
								std::cout << "player[" << main_player << "]:pung\n\n";
								tempcard[2] = outcard;
								sendtoother(client, tempcard, 3, "pungcard", "card");
							}

						}
						else {
							getjsonvalue(r[dgun], tempcard, 3, false, "gun");
							if (Sendcard(tempcard[0], tempcard[1], tempcard[2], outcard) == 1) {
								main_player = dgun;
								player_do = true;
								tempcard[3] = outcard;
								sendtoother(client, tempcard, 4, "guncard", "card");
							}
						}

					}
					else if (deat != -1) { //吃牌
						int tempcard[3];
						getjsonvalue(r[deat], tempcard, 2, false, "eat");
						tempcard[2] = outcard;
						std::cout << "eatcard:" << tempcard[0] << " " << tempcard[1] << " " << outcard << std::endl;
						if (Sendcard(tempcard[0], tempcard[1], outcard, false) == 1) {
							main_player = deat;
							player_do = true;
							std::cout << "player[" << main_player << "]:eat\n\n";
							sendtoother(client, tempcard, 3, "eatcard", "card");
						}
					}

					if (player_do) {
						//read "outcard"
						json doj,dod;
						dod["done"];
						client.at(main_player)->read(doj);
						client.at(main_player)->send(dod);
						outcard = doj["outcard"];
					}
					else { //都沒事,下個使用者
						main_player = (main_player + 1) % 4;
						break;
					}

				}//連續吃碰槓while's

				
				 //one check he self who
				if (who == true) { //自摸成立
					break;
				}
			}//this is playing while's
			
			try {
				//處理胡牌
				if (who == true) {
					/*
					std::cout << "server whocard:";
					for (int i = 0; i < 17; i++) {
						std::cout << whocard[i] << " ";

					}
					*/

					sendtoother(client, whocard, 17, "whocard", "card");
				}
			}
			catch (...) {
				std::cout << "get exception.\n";
				for (int i = 0; i < 17; i++)
					std::cout << whocard[i] << " ";
			}
			

			endclient(client);

		}//this is onegame's
		
	};
};

#endif 