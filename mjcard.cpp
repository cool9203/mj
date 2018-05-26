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
	std::string dcm = "�U���������]�O";
	std::string chinesemath = "�@�G�T�|�����C�K�E�F�n��_���o��";
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


//�Y�I�b�ɤ~�Q�γofunc �������諸�d�ಾ�A��®���P���ܴN��push()�N�n
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
//�����禡,return index , can throw;
int mjcard::card_price(int *tempcard, int len) {
	//�o�䤣��ƻscard array  �p�G�ΰƥ��Ӱ�sort��A�^�Ǫ�index�|�O���~��
	//��sort�@�M
	card_sort(tempcard, len, true);

	//0.-�����W���r�P
	for (int i = 0; i < len; i++) {
		if (tempcard[i] >= 31) { //�p�G�O�r�P
			if (getcardtimes(tempcard, len, tempcard[i]) >= 2) { //�p�G>=2�i
				continue;
			}
			else {
				return i;
			}
		}
	}

	//1.-�A��t�i
	for (int i = 0; i < len; i++) {
		if (tempcard[i] < 30) { //�p�G���O�r�P than
			if (card_expect(tempcard, len, i) == false) //�p�G�S���i�H�������� or ��� than
				return i;
		}
	}

	//2.-�u���������禡-�����
	int index = card_expect(tempcard, len, card_times);
	if (index != -1) {
		return index;
	}

	/*�n�ɧ@��O�H��L���P*/

	return 0;

}


//���X�w�����������,return ���ܫ᪺len����
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


//card_organize���l�禡�A�ΨӮ��X��and��ը�᭱�Areturn �`�����
int mjcard::card_organize(int *tempcard, int len, std::vector<int> &buf, bool smallorder, bool preorder) {
	int m, buf_order[17], buf_pair[17], optr = 0, pptr = 0, _buf[17], bptr = 0;
	//clean
	for (int i = 0; i < 17; i++) {
		buf_order[i] = -1;
		buf_pair[i] = -1;
		_buf[i] = -1;
	}
	//���ƧǤ@�M
	card_sort(tempcard, len, smallorder);
	//set m  �̷ӱƦC����,m�n����ܴ�
	if (smallorder) {
		m = 1;
	}
	else {
		m = -1;
	}
	//�M�w���������٬O�������
	if (preorder) {
		optr = card_check_order(tempcard, len, buf_order, m);
		pptr = card_check_pair(tempcard, len, buf_order, optr, buf_pair);
	}
	else {
		pptr = card_check_pair(tempcard, len, buf_pair);
		optr = card_check_order(tempcard, len, buf_pair, pptr, buf_order, m);
	}


	buf.push_back(len - optr - pptr); //���j�I
									  //�n��X�̫ᨺ2�i�����A�ðO����buf[]��
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


//�Ǧ^�`�����
int mjcard::get_card_expect(int *tcard, int len) {
	std::vector<int> calculated, expect;
	int tempcard[17];
	card_cpy(tcard, tempcard, len);

	card_sort(tempcard, len, true); //���Ƨ�-�p��j

								//�o�䤣�Q�שt�i�A�]���b�W�@�h��function�w�g�P�_�L�F
	for (int i = 0; i < len - 1; i++) {
		int temp = 0;
		if (!card_level(tempcard[i], tempcard[i + 1])) { //�p�G���b�P�Ӷq�Ū��� than
			continue;
		}
		if (tempcard[i + 1] - tempcard[i] >= 3) {
			continue;
		}
		if (tempcard[i] <= 0) {
			continue;
		}

		if (tempcard[i] == tempcard[i + 1]) { //�p�G�O2�i��ժ��� than
			if (search_vector(calculated, tempcard[i]))
				continue;
			expect.push_back(card_times[tempcard[i]]);
			calculated.push_back(tempcard[i]);
		}
		else { //���O��ը��N�O���դF�A�p�G�O���ժ��� than
			if (tempcard[i] > 30)
				continue;
			if (tempcard[i + 1] - tempcard[i] == 1) { //�p�G�O 34 �o�����P�� than
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
			else { //�p�G�O 35�o�����P�� than
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


//��X����ȥH�K��P,return �n�᪺�P��index
int mjcard::card_expect(int *tempcard, int len, int *cardtimes) {
	std::vector<int> buf, calculated, expect;
	int times[44]; //�ݥX�{���ơA�����X�{���Ƥ֪��Q��X�h�A�ߤ��n����graph
	for (int i = 0; i < 44; i++)
		times[i] = 0;

	card_sort(tempcard, len, true); //���Ƨ�-�p��j

								//�o�䤣�Q�שt�i�A�]���b�W�@�h��function�w�g�P�_�L�F
	for (int i = 0; i < len - 1; i++) {
		int temp = 0;
		if (!card_level(tempcard[i], tempcard[i + 1])) { //�p�G���b�P�Ӷq�Ū��� than
			expect.push_back(99);
			continue;
		}
		if (tempcard[i + 1] - tempcard[i] >= 3) {
			expect.push_back(99);
			times[tempcard[i]]++;
			times[tempcard[i + 1]]++;
			continue;
		}


		if (tempcard[i] == tempcard[i + 1]) { //�p�G�O2�i��ժ��� than
			expect.push_back(cardtimes[tempcard[i]]);
		}
		else { //���O��ը��N�O���դF�A�p�G�O���ժ��� than
			if (tempcard[i + 1] - tempcard[i] == 1) { //�p�G�O 34 �o�����P�� than
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
			else { //�p�G�O 35�o�����P�� than
				expect.push_back(cardtimes[tempcard[i] + 1]);
			}
		}
		//�X�{����++
		times[tempcard[i]]++;
		times[tempcard[i + 1]]++;
	}

	int minexpect = search_vextor_min(expect); //�����o�̤p����Ȫ����ӹ�or����
	int min = search_array_min(times, len); //���o�X�{���Ƥp��
											//�����ȧC+�s�����Ƥ�
	for (int i = 0; i < static_cast<int>(expect.size()); i++) {
		//�p�G����ȳ̧C�A�ӥB�X�{���Ƥ]�̤p�A�Nreturn index
		if (minexpect == -1)
			break;

		if (times[tempcard[minexpect]] == min)
			return minexpect;
		if (times[tempcard[minexpect + 1]] == min)
			return minexpect + 1;

		minexpect = search_vector_index(expect, minexpect + 1, expect.at(minexpect)); //�U�@�Ӹ��C����Ȫ�index
	}

	//�����ȧC��
	minexpect = search_vextor_min(expect); //���s���o�̤p����Ȫ����or����
										   //��X���a1&9���Ʀr
	if (abs(5 - tempcard[minexpect]) > abs(5 - tempcard[minexpect + 1])) //ex:abs(5-1)>abs(5-2)
		return minexpect;
	else if (abs(5 - tempcard[minexpect]) < abs(5 - tempcard[minexpect + 1]))
		return minexpect + 1;
	return -1;
}


//�o�i�P�A���S���զ��i�H���ݪ���or�� ex:input 5  search for 3 4 5 6 7
//cptr�����w��index
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
	//�ݦ��S���i�H��������
	for (int i = 0; i < static_cast<int>(buf.size()); i++) {
		if (cmp(tempcard, len, 0, buf.at(i)) != -1)
			return true;
	}
	//�ݦ��S���i�H�������
	if (getcardtimes(tempcard, len, tempcard[cptr]) >= 2)
		return true;

	return false;
}


//�P�_�o��i�P�O���O�b�P�Ӷq��
//yes return true     not return false
bool mjcard::card_level(int card1, int card2) {
	if (card1 < 10 && card1>0 && card2 < 10 && card2>0) { //�U�r
		return true;
	}
	if (card1 < 20 && card1>10 && card2 < 20 && card2>10) { //���l
		return true;
	}
	if (card1 < 30 && card1>20 && card2 < 30 && card2>20) { //���l
		return true;
	}

	return false;
}


//�^��*data�̪��̤p��
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


//��buf�̤p�� return index
int mjcard::search_vextor_min(std::vector<int> &buf) {
	int min = 0;
	for (int i = 0; i < static_cast<int>(buf.size()); i++) {
		if (buf.at(min) > buf.at(i))
			min = i;
	}
	return min;
}


//��assgin���Ȧ��S���bbuf�̥X�{�L
bool mjcard::search_vector(std::vector<int> &buf, int assign) {
	for (int i = 0; i < static_cast<int>(buf.size()); i++) {
		if (buf.at(i) == assign)
			return true;
	}
	return false;
}


//��Xvector buf�̪����w��,return index
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


//���o���w�Ʀrm�P�̥X�{�X���A
int mjcard::getcardtimes(int *tempcard, int len, int m) {
	int count = 0;
	for (int i = 0; i < len; i++) {
		if (tempcard[i] == m) {
			count++;
		}
	}
	return count;
}


//�ݯʤ���P
void mjcard::set_need_card() {
	int len = cptr;
	int tcard[17];
	card_cpy(card, tcard, len);
	card_sort(tcard, len, true); //���Ƨ�-�p��j
	need_card.clear();
	need_card_index.clear();

	//�o�䤣�Q�שt�i�A�]���b�W�@�h��function�w�g�P�_�L�F
	for (int i = 0; i < len - 1; i++) {
		int temp = 0;
		if (!card_level(tcard[i], tcard[i + 1])) { //�p�G���b�P�Ӷq�Ū��� than
			continue;
		}
		if (tcard[i + 1] - tcard[i] >= 3) { //�p�G�L�k������ than
			continue;
		}

		if (tcard[i] == tcard[i + 1]) { //�p�G�O2�i��ժ��� than
			need_card.push_back(tcard[i]); //push��ڼƦr

		}
		else { //���O��ը��N�O���դF�A�p�G�O���ժ��� than
			if (tcard[i + 1] - tcard[i] == 1) { //�p�G�O 34 �o�����P�� than
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
			else { //�p�G�O 35�o�����P�� than
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