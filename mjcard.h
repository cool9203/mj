#pragma once
#include <vector>
#include <string>
#include "mj.h"

class mjcard : public mj{
protected:
	int card[17];
	int _ocard[17];
	int playernumber;
	std::vector<int> need_card, need_card_index;
	std::string need_status;


	int cptr;
	int _ocptr;

	void transfer(int);
	int card_merge(int*);
	void print_c_card(int*, int);

#pragma region AI
	int card_times[44];

	int card_price(int*, int);

	void card_grouping();
	int card_organize(int*, int, std::vector<int>&, bool, bool);

	int get_card_expect(int*, int);
	int card_expect(int*, int, int*);
	bool card_expect(int*, int, int);

	bool card_level(int, int);

	int search_array_min(int*, int);
	int search_vextor_min(std::vector<int>&);
	bool search_vector(std::vector<int>&, int);
	int search_vector_index(std::vector<int>&, int, int);

	int getcardtimes(int*, int, int);

#pragma endregion</spin>

public:
	mjcard();
	mjcard(int*, int);

	void push(int);
	int pop();
	int pop(int);
	void swap(int, int);
	int find(int);
	void arrange();

	bool check_who();
	bool check_who(int);

	void print();
	void print_o();
	void print_c();
	void print_o_c();

	int* get_card();
	int get_cptr();
	int get_ocptr();

#pragma region AI
	int need(int*,int);
	std::vector<int> need(int);
	std::string get_need_status();
	void get(int,int,int);
	int out();

	void set_mycardtimes(int*, int);
	void set_mycardtimes(int);

	void set_need_card();

#pragma endregion</spin>
};