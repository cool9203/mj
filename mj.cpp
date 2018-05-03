#include <algorithm>
#include "mj.h"


#pragma region 確認是否胡牌
//card_check第三層，找出順子，並回傳是否有此順組，沒有則-1，有則>1
//p為開始的index，m為指定要找的昰遞增或遞減
int mj::order_check(int *card, int *order, int len, int orderlen, int *buf, int bptr, int start, int m) {
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
int mj::pair_appear(int *card, int *pair, int len, int pairlen, int *buf, int bptr, int start) {
	return this->order_check(card, pair, len, pairlen, buf, bptr, start, 0);
}


//card_check第二層，順組的檢查
int mj::card_check_order(int *card, int cardlen, int *buf, int m) {
	int pair[17];
	for (int i = 0; i < 17; i++) {
		pair[i] = -1;
	}

	return this->card_check_order(card, cardlen, pair, 17, buf, m);
}
int mj::card_check_order(int *card, int cardlen, int *pair, int pairlen, int *buf, int m) {
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

	return bptr;
}


//card_check第二層，對組的檢查
int mj::card_check_pair(int *card, int cardlen, int *buf) {
	int order[17];
	for (int i = 0; i < 17; i++) {
		order[i] = -1;
	}
	return this->card_check_pair(card, cardlen, order, 17, buf);
}
int mj::card_check_pair(int *card, int cardlen, int *order, int orderlen, int *buf) {
	return this->card_check_order(card, cardlen, order, orderlen, buf, 0);
}


//看有沒有等於m值的，有的話回傳他的位置，沒有則回傳-1
//m為指定數字
int mj::cmp(int *card, int len, int start, int m) {
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
int mj::card_check_check(int *card, int len, bool smallorder, bool preorder) {
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

	//看是不是真的最後2張當眼的是一樣的牌
	if (bptr < 2 || bptr > 2 || card[buf[0]] != card[buf[1]]) {
		return -1;
	}


	return 1;
}


//第0層
//因為檢查胡牌要判斷4種情況，所以在這統一處理
//胡牌return 1   沒胡牌return 0
int mj::card_check(int *tempcard, int len) {
	int card[17];
	card_cpy(tempcard, card, len);

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
void mj::card_sort(int *card, int len, bool SmalltoBig) {
	std::sort(card, card + len);
	if (!SmalltoBig)
		std::reverse(card, card + len);
}


//cardcopy
void mj::card_cpy(int *tempcard, int *card, int len) {
	for (int i = 0; i < 17; i++)
		card[i] = -1;
	for (int i = 0; i < len; i++)
		card[i] = tempcard[i];
}