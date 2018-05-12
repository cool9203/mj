#pragma once

class mj {
protected:
	int card_check_check(int*, int, bool, bool);

	int card_check_order(int*, int, int*, int);
	int card_check_order(int*, int, int*, int,int*,int);

	int card_check_pair(int*, int, int*);
	int card_check_pair(int*, int, int*,int,int*);

	int order_check(int*, int*, int, int, int*, int, int, int);
	int pair_appear(int*, int*, int, int, int*, int, int);

public:
	int cmp(int*, int, int, int);
	void card_cpy(int*,int*,int);
	void card_sort(int*, int, bool Small_to_big);
	int card_check(int*, int);
};