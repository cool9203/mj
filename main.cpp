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
#pragma comment(lib, "Ws2_32.lib")

void server() {
	MJSystem mj;
	mj.start();
}

void client() {
	Player p;
	p.start();
	std::cout << "p.closr()\n";
}

int main(int argc, char *argv[]) {
	
	
	while (true) {
		try {
			int c[17] = { 1,1,1,2,2,2,3,3,3,4,4,5,5,6,6,7,7 };
			mjcard card(c, 17);

			/*
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
			*/
		}
		catch (...) {
			abort();
		}

		int p=1;
		//std::cin >> p;

		if (p == 1)
			break;
	}
	
	

	system("PAUSE");

	return 0;
}

