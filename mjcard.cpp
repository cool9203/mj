#include <iostream>
#include "mjcard.h"


mjcard::mjcard() {
	for (int i = 0; i < 17; i++) {
		card[i] = -1;
		_ocard[i] = -1;
	}
	for (int i = 0; i < 44; i++) {
		card_times[i] = 4;
	}
	cptr = 0;
	_ocptr = 0;
	need_status = "NULL";
	playernumber = -1;
}


mjcard::mjcard(int *data, int datalen) : mjcard() { //: mjcard() this is can call constructor
	for (int i = 0; i < datalen; i++) {
		push(data[i]);
	}
}


void mjcard::transfer(int data) {
	if (!find(data))
		throw std::invalid_argument("transfer no find match");

	cptr--;
	swap(find(data), cptr);
	_ocard[_ocptr] = card[cptr];
	_ocptr++;
	card[cptr] = -1;
}


//return _buf len
int mjcard::card_merge(int *_buf) {
	int len = 0;
	for (int i = 0; i < cptr; i++) {
		_buf[len] = card[i];
		len++;
	}
	for (int i = 0; i < _ocptr; i++) {
		_buf[len] = _ocard[i];
		len++;
	}

	return len;
}


//print card to chinese
void mjcard::print_c_card(int *tempcard, int cardlen) {
	std::string stack;
	std::string dcm = "萬條筒風中財板";
	std::string chinesemath = "一二三四五六七八九東南西北紅發白";
	std::cout << "\n";
	for (int i = 0; i < cardlen; i++) {
		char c[3];
		if (tempcard[i] < 30) {
			c[0] = chinesemath.at((tempcard[i] % 10 - 1) * 2);
			c[1] = chinesemath.at((tempcard[i] % 10 - 1) * 2 + 1);
			c[2] = '\0';
		}
		else {
			c[0] = chinesemath.at((tempcard[i] % 31) + 18);
			c[1] = chinesemath.at((tempcard[i] % 31) + 19);
			c[2] = '\0';
		}

		std::cout << c << "  ";

		if (tempcard[i] < 10) {
			stack.push_back(0);
		}
		else if (tempcard[i] < 20 && tempcard[i] > 10) {
			stack.push_back(2);
		}
		else if (tempcard[i] < 30 && tempcard[i] > 20) {
			stack.push_back(4);
		}
		else if (tempcard[i] < 38 && tempcard[i] > 30) {
			stack.push_back(6);
		}
		else if (tempcard[i] == 39) {
			stack.push_back(8);
		}
		else if (tempcard[i] == 41) {
			stack.push_back(10);
		}
		else if (tempcard[i] == 43) {
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


void mjcard::push(int data) {
	card[cptr] = data;
	cptr++;
	set_mycardtimes(data);
}


int mjcard::pop() {
	cptr--;
	int temp = card[cptr];
	card[cptr] = -1;
	set_need_card();
	return temp;
}


int mjcard::pop(int index) {
	cptr--;
	swap(index, cptr);
	int temp = card[cptr];
	card[cptr] = -1;
	set_need_card();
	return temp;
}


void mjcard::swap(int index_1,int index_2) {
	std::swap(card[index_1], card[index_2]);
}


int mjcard::find(int assign) {
	for (int i = 0; i < cptr; i++) {
		if (assign == card[i]) {
			return i;
		}
	}
	return -1;
}


void mjcard::arrange() {
	card_sort(card, cptr, true);
	card_grouping();
	set_need_card();
}


bool mjcard::check_who() {
	return card_check(card, cptr);

}


bool mjcard::check_who(int data) {
	int tempcard[17];
	if ((cptr + _ocptr) == 17)
		return false;
	card_cpy(card, tempcard, cptr);
	tempcard[cptr] = data;
	return card_check(tempcard, cptr + 1);
}


void mjcard::print() {
	for (int i = 0; i < cptr; i++) {
		std::cout << card[i] << " ";
	}
	std::cout << "\n";
}


void mjcard::print_o() {
	for (int i = 0; i < _ocptr; i++) {
		std::cout << _ocard[i] << " ";
	}
	std::cout << "\n";
}


void mjcard::print_c() {
	print_c_card(card, cptr);
}


void mjcard::print_o_c() {
	print_c_card(_ocard, _ocptr);
}


std::vector<int> mjcard::get_card() {
	std::vector<int> v;
	int tempcard[17];
	int len = card_merge(tempcard);
	for (int i = 0; i < len; i++) {
		v.push_back(tempcard[i]);
	}
	return v;
}


int mjcard::get_cptr() {
	return cptr;
}


int mjcard::get_ocptr() {
	return _ocptr;
}


int mjcard::need(int *_buf,int assign) {
	std::vector<int> v = need(assign);
	if (v.size() >= 2)
		return -1;

	for (int i = 0; i < static_cast<int>(v.size()); i++) {
		_buf[i] = v.at(i);
	}
	return static_cast<int>(v.size());
}


std::vector<int> mjcard::need(int assign) {
	std::vector<int> vc;
	need_status.clear();
	for (int i = 0; i < static_cast<int>(need_card.size()); i++) {
		if (assign == need_card.at(i)) {
			vc.push_back(card[need_card_index.at(i * 2)]);
			vc.push_back(card[need_card_index.at(i * 2 + 1)]);
			if (vc.at(0) == vc.at(1)) {
				need_status = "pung";
			}
			else {
				need_status = "eat";
			}
			return vc;
		}
	}
	need_status = "NULL";
	return vc;
}


std::string mjcard::get_need_status() {
	return need_status;
}


//吃碰槓時才利用這func 來讓成對的卡轉移，單純拿到牌的話就用push()就好
void mjcard::get(int data1,int data2,int getdata) {
	transfer(data1);
	transfer(data2);
	_ocard[_ocptr] = getdata;
	_ocptr++;
}


int mjcard::out() {
	int index = card_price(card, cptr);
	int outcard = card[index];
	cptr--;
	swap(index, cptr);
	card[cptr] = -1;
	return outcard;
}


#pragma region AI
//估價函式,return index , can throw;
int mjcard::card_price(int *tempcard, int len) {
	//這邊不能複製card array  如果用副本來做sort後，回傳的index會是錯誤的
	//先sort一遍
	card_sort(tempcard, len, true);

	//0.-先丟單獨的字牌
	for (int i = 0; i < len; i++) {
		if (tempcard[i] >= 31) { //如果是字牌
			if (getcardtimes(tempcard, len, tempcard[i]) >= 2) { //如果>=2張
				continue;
			}
			else {
				return i;
			}
		}
	}

	//1.-再丟孤張
	for (int i = 0; i < len; i++) {
		if (tempcard[i] < 30) { //如果不是字牌 than
			if (card_expect(tempcard, len, i) == false) //如果沒有可以等的順組 or 對組 than
				return i;
		}
	}

	//2.-真正的估價函式-期望值
	int index = card_expect(tempcard, len, card_times);
	if (index != -1) {
		return index;
	}

	/*要補作丟別人丟過的牌*/

	return 0;

}


//拿出已完成的順對組,return 改變後的len的值
void mjcard::card_grouping() {
	std::vector<int> buf[4];
	int expect[4];
	expect[0] = card_organize(card, cptr, buf[0], true, true);
	expect[1] = card_organize(card, cptr, buf[1], true, false);
	expect[2] = card_organize(card, cptr, buf[2], false, true);
	expect[3] = card_organize(card, cptr, buf[3], false, false);

	int max = 0;
	for (int i = 0; i < 4; i++) {
		if (expect[max] < expect[i]) {
			max = i;
		}
	}

	for (int i = 1; i < static_cast<int>(buf[max].size()); i++) {
		card[i - 1] = buf[max].at(i);
	}

	cptr = buf[max].at(0);

	for (int i = cptr; i < 17; i++) {
		if (card[i] != -1) {
			_ocard[_ocptr] = card[i];
			card[i] = -1;
			_ocptr++;
		}
	}
}


//card_organize的子函式，用來拿出順and對組到後面，return 總期望值
int mjcard::card_organize(int *tempcard, int len, std::vector<int> &buf, bool smallorder, bool preorder) {
	int m, buf_order[17], buf_pair[17], optr = 0, pptr = 0, _buf[17], bptr = 0;
	//clean
	for (int i = 0; i < 17; i++) {
		buf_order[i] = -1;
		buf_pair[i] = -1;
		_buf[i] = -1;
	}
	//先排序一遍
	card_sort(tempcard, len, smallorder);
	//set m  依照排列順序,m要跟著變換
	if (smallorder) {
		m = 1;
	}
	else {
		m = -1;
	}
	//決定先取順組還是先取對組
	if (preorder) {
		optr = card_check_order(tempcard, len, buf_order, m);
		pptr = card_check_pair(tempcard, len, buf_order, optr, buf_pair);
	}
	else {
		pptr = card_check_pair(tempcard, len, buf_pair);
		optr = card_check_order(tempcard, len, buf_pair, pptr, buf_order, m);
	}


	buf.push_back(len - optr - pptr); //分隔點
									  //要找出最後那2張當眼的，並記錄到buf[]裡
	for (int i = 0; i < len; i++) {
		if (cmp(buf_order, optr, 0, i) == -1 && cmp(buf_pair, pptr, 0, i) == -1) {
			buf.push_back(tempcard[i]);
			_buf[bptr] = tempcard[i];
			bptr++;
		}
	}
	for (int i = 0; i < optr; i++) {
		buf.push_back(tempcard[buf_order[i]]);
	}
	for (int i = 0; i < pptr; i++) {
		buf.push_back(tempcard[buf_pair[i]]);
	}

	return get_card_expect(_buf, bptr);
}


//傳回總期望值
int mjcard::get_card_expect(int *tcard, int len) {
	std::vector<int> calculated, expect;
	int tempcard[17];
	card_cpy(tcard, tempcard, len);

	card_sort(tempcard, len, true); //先排序-小到大

								//這邊不討論孤張，因為在上一層的function已經判斷過了
	for (int i = 0; i < len - 1; i++) {
		int temp = 0;
		if (!card_level(tempcard[i], tempcard[i + 1])) { //如果不在同個量級的話 than
			continue;
		}
		if (tempcard[i + 1] - tempcard[i] >= 3) {
			continue;
		}
		if (tempcard[i] <= 0) {
			continue;
		}

		if (tempcard[i] == tempcard[i + 1]) { //如果是2張對組的話 than
			if (search_vector(calculated, tempcard[i]))
				continue;
			expect.push_back(card_times[tempcard[i]]);
			calculated.push_back(tempcard[i]);
		}
		else { //不是對組那就是順組了，如果是順組的話 than
			if (tempcard[i] > 30)
				continue;
			if (tempcard[i + 1] - tempcard[i] == 1) { //如果是 34 這類的牌型 than
				if (tempcard[i] % 10 == 1) {
					if (search_vector(calculated, tempcard[i + 1] + 1))
						continue;
					expect.push_back(card_times[tempcard[i + 1] + 1]);
					calculated.push_back(tempcard[i + 1] + 1);

				}
				else if (tempcard[i] % 10 == 8) {
					if (search_vector(calculated, tempcard[i] - 1))
						continue;
					expect.push_back(card_times[tempcard[i] - 1]);
					calculated.push_back(tempcard[i] - 1);

				}
				else {
					if (!search_vector(calculated, tempcard[i] - 1)) {
						temp += card_times[tempcard[i] - 1];
						calculated.push_back(tempcard[i] - 1);
					}
					if (!search_vector(calculated, tempcard[i + 1] + 1)) {
						temp += card_times[tempcard[i + 1] + 1];
						calculated.push_back(tempcard[i + 1] + 1);
					}
					expect.push_back(temp);
				}
			}
			else { //如果是 35這類的牌型 than
				if (search_vector(calculated, tempcard[i] + 1))
					continue;
				expect.push_back(card_times[tempcard[i] + 1]);
				calculated.push_back(tempcard[i] + 1);
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
int mjcard::card_expect(int *tempcard, int len, int *cardtimes) {
	std::vector<int> buf, calculated, expect;
	int times[44]; //看出現次數，來讓出現次數少的被丟出去，心中要有個graph
	for (int i = 0; i < 44; i++)
		times[i] = 0;

	card_sort(tempcard, len, true); //先排序-小到大

								//這邊不討論孤張，因為在上一層的function已經判斷過了
	for (int i = 0; i < len - 1; i++) {
		int temp = 0;
		if (!card_level(tempcard[i], tempcard[i + 1])) { //如果不在同個量級的話 than
			expect.push_back(99);
			continue;
		}
		if (tempcard[i + 1] - tempcard[i] >= 3) {
			expect.push_back(99);
			times[tempcard[i]]++;
			times[tempcard[i + 1]]++;
			continue;
		}


		if (tempcard[i] == tempcard[i + 1]) { //如果是2張對組的話 than
			expect.push_back(cardtimes[tempcard[i]]);
		}
		else { //不是對組那就是順組了，如果是順組的話 than
			if (tempcard[i + 1] - tempcard[i] == 1) { //如果是 34 這類的牌型 than
				if (tempcard[i] % 10 == 1) {
					expect.push_back(cardtimes[tempcard[i + 1] + 1]);
				}
				else if (tempcard[i] % 10 == 8) {
					expect.push_back(cardtimes[tempcard[i] - 1]);
				}
				else {
					temp += cardtimes[tempcard[i] - 1];
					temp += cardtimes[tempcard[i + 1] + 1];
					expect.push_back(temp);
				}
			}
			else { //如果是 35這類的牌型 than
				expect.push_back(cardtimes[tempcard[i] + 1]);
			}
		}
		//出現次數++
		times[tempcard[i]]++;
		times[tempcard[i + 1]]++;
	}

	int minexpect = search_vextor_min(expect); //先取得最小期望值的那個對or順組
	int min = search_array_min(times, len); //取得出現次數小的
											//丟期望值低+連結次數少
	for (int i = 0; i < static_cast<int>(expect.size()); i++) {
		//如果期望值最低，而且出現次數也最小，就return index
		if (minexpect == -1)
			break;

		if (times[tempcard[minexpect]] == min)
			return minexpect;
		if (times[tempcard[minexpect + 1]] == min)
			return minexpect + 1;

		minexpect = search_vector_index(expect, minexpect + 1, expect.at(minexpect)); //下一個較低期望值的index
	}

	//丟期望值低的
	minexpect = search_vextor_min(expect); //重新取得最小期望值的對組or順組
										   //丟出較靠1&9的數字
	if (abs(5 - tempcard[minexpect]) > abs(5 - tempcard[minexpect + 1])) //ex:abs(5-1)>abs(5-2)
		return minexpect;
	else if (abs(5 - tempcard[minexpect]) < abs(5 - tempcard[minexpect + 1]))
		return minexpect + 1;
	return -1;
}


//這張牌，有沒有組成可以等待的順or對 ex:input 5  search for 3 4 5 6 7
//cptr為指定的index
bool mjcard::card_expect(int *tempcard, int len, int cptr) {
	std::vector<int> buf;
	if (tempcard[cptr] % 10 == 1) {
		buf.push_back(tempcard[cptr] + 1);
		buf.push_back(tempcard[cptr] + 2);

	}
	else if (tempcard[cptr] % 10 == 9) {
		buf.push_back(tempcard[cptr] - 2);
		buf.push_back(tempcard[cptr] - 1);

	}
	else if (tempcard[cptr] % 10 == 2) {
		buf.push_back(tempcard[cptr] - 1);
		buf.push_back(tempcard[cptr] + 1);
		buf.push_back(tempcard[cptr] + 2);

	}
	else if (tempcard[cptr] % 10 == 8) {
		buf.push_back(tempcard[cptr] - 2);
		buf.push_back(tempcard[cptr] - 1);
		buf.push_back(tempcard[cptr] + 1);

	}
	else {
		buf.push_back(tempcard[cptr] - 2);
		buf.push_back(tempcard[cptr] - 1);
		buf.push_back(tempcard[cptr] + 1);
		buf.push_back(tempcard[cptr] + 2);
	}
	//看有沒有可以等的順組
	for (int i = 0; i < static_cast<int>(buf.size()); i++) {
		if (cmp(tempcard, len, 0, buf.at(i)) != -1)
			return true;
	}
	//看有沒有可以等的對組
	if (getcardtimes(tempcard, len, tempcard[cptr]) >= 2)
		return true;

	return false;
}


//判斷這兩張牌是不是在同個量級
//yes return true     not return false
bool mjcard::card_level(int card1, int card2) {
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


//回傳*data裡的最小值
int mjcard::search_array_min(int *data, int len) {
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
int mjcard::search_vextor_min(std::vector<int> &buf) {
	int min = 0;
	for (int i = 0; i < static_cast<int>(buf.size()); i++) {
		if (buf.at(min) > buf.at(i))
			min = i;
	}
	return min;
}


//找assgin的值有沒有在buf裡出現過
bool mjcard::search_vector(std::vector<int> &buf, int assign) {
	for (int i = 0; i < static_cast<int>(buf.size()); i++) {
		if (buf.at(i) == assign)
			return true;
	}
	return false;
}


//找出vector buf裡的指定值,return index
int mjcard::search_vector_index(std::vector<int> &buf, int start, int assign) {
	for (int i = start; i < static_cast<int>(buf.size()); i++) {
		if (buf.at(i) == assign)
			return i;
	}
	return -1;
}


void mjcard::set_mycardtimes(int *tempcard, int len) {
	for (int i = 0; i < len; i++) {
		if (tempcard[i] == -1)
			continue;
		card_times[tempcard[i]]--;
	}
}


void mjcard::set_mycardtimes(int tempcard) {
	if (!tempcard)
		return;
	card_times[tempcard]--;
}


void mjcard::set_mycardtimes(std::vector<int> data) {
	if (data.size() == 0)
		return;
	for (std::vector<int>::iterator it = data.begin(); it != data.end(); ++it) {
		card_times[*it]--;
	}
}


//取得指定數字m牌裡出現幾次，
int mjcard::getcardtimes(int *tempcard, int len, int m) {
	int count = 0;
	for (int i = 0; i < len; i++) {
		if (tempcard[i] == m) {
			count++;
		}
	}
	return count;
}


//看缺什麼牌
void mjcard::set_need_card() {
	int len = cptr;
	int tcard[17];
	card_cpy(card, tcard, len);
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
	/*
	for (int i = 0; i < static_cast<int>(need_card.size()); i++)
		std::cout << need_card.at(i) << " ";

	std::cout << "\n\n";

	for (int i = 0; i < static_cast<int>(need_card_index.size()); i++)
		std::cout << card[need_card_index.at(i)] << " ";
	*/
}
#pragma endregion</spin>