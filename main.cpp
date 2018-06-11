#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <mutex>
#include <windows.h>
#include <sstream>
#include <string>
#include <iostream>
#include "mjcard.h"
#include "MJsystem.h"
#include "Player.h"
#include "mjtransmission.h"
#pragma comment(lib, "Ws2_32.lib")

void server() {
	MJSystem mj;
	mj.start();
}

void client() {
	Player p;
	p.start();
	//std::cout << "p.closr()\n";
}


int main(int argc, char *argv[]) {
	
	int card[] = {15,17,17,18,19};
	//int card[] = { 31,21,9,16,25,8,11,31,8,15,19,5,8,6,6,6 };
	//int card[] = { 9,11,13,21,23,27,27 };
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
	/*
	mjcard mj(card, 5);
	mj.set_need_card();
	while (true) {
		int m;
		std::cin >> m;
		std::vector<int> v = mj.need(m);
		for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << mj.get_need_status();
		std::cout << "\n\n";
	}
	*/
	
	
	
	while (true) {
		try {
			
			std::thread s(server);
			std::thread c1(client);
			std::thread c2(client);
			std::thread c3(client);
			std::thread c4(client);
			c1.join();
			c2.join();
			c3.join();
			c4.join();
			s.join();

			s.~thread();
			c1.~thread();
			c2.~thread();
			c3.~thread();
			c4.~thread();
			
		}
		catch (...) {
			abort();
		}

		int p=0;
		//std::cin >> p;

		if (p == 1)
			break;
	}
	
	

	system("PAUSE");

	return 0;
}

