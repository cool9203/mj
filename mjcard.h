#pragma once
#include <vector>
#include "mj.h"

class mjcard : public mj{
protected:
	int card[17];
	int _ocard[17];
	std::vector<int> need_card, need_card_index;

	int cptr;
	int _ocptr;

	int card_merge(int*);
	void print_c_card(int*, int);

#pragma region AI
	int card_times[44];

	int card_price(int*, int);

	int card_grouping(int*, int);
	int card_organize(int*, int, std::vector<int>&, bool, bool);

	int get_card_expect(int*, int);
	int card_expect(int*, int, int*);
	bool card_expect(int*, int, int);

	bool card_level(int, int);

	int search_array_min(int*, int);
	int search_vextor_min(std::vector<int>&);
	bool search_vector(std::vector<int>&, int);
	int search_vector_index(std::vector<int>&, int, int);

	void set_mycardtimes(int*, int);
	void set_mycardtimes(int);
	int getcardtimes(int*, int, int);

	void get_need_card(int*, int);

#pragma endregion</spin>

public:
	mjcard();
	mjcard(int*, int);

	void push(int);
	int pop();
	int pop(int);
	void swap(int, int);

	bool check_who();

	void print_card();
	void print_ocard();
	void print_card_c();
	void print_ocard_c();

	int get_card_ptr();
	int get_ocard_ptr();

#pragma region AI
	int need(int);
	void get();
	int out();

#pragma endregion</spin>
};