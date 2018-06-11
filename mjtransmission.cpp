#include "mjtransmission.h"

json mjtransmission::done() {
	json d;
	d["done"];
	return d;
}


//get std::string
std::string mjtransmission::getstri(const char *name, int intdata) {
	std::string str = name, temp;
	std::stringstream ss;
	ss << intdata;
	ss >> temp;
	str += temp;

	return str;
}

//int to json
json mjtransmission::iarrtojson(std::vector<int> &data, const char *main_json_name, const char *second_json_name) {
	return iarrtojson(data, main_json_name, second_json_name, false, -1);
}


json mjtransmission::iarrtojson(std::vector<int> &data, const char *main_json_name, const char *second_json_name, bool sendplayernumber, int playernumber) {
	json j, secj;

	if (sendplayernumber && playernumber) {
		secj["playernumber"] = playernumber;
	}
	else if (sendplayernumber && playernumber < 0) {
		throw "player_number_error";
	}

	for (int i = 0; i < static_cast<int>(data.size()); i++) {
		secj[getstri(second_json_name, i + 1)] = data.at(i);
	}
	j[main_json_name] = secj;

	return j;
}


//return playernumber
int mjtransmission::getjsonvalue(json &tj, std::vector<int> &buf ,int getlen, const char *tjname) {
	return getjsonvalue(tj, buf, getlen, tjname, "card");
}


int mjtransmission::getjsonvalue(json &tj, std::vector<int> &buf, int getlen, const char *tjname, const char *getname) {
	json j = tj[tjname];
	int playernumber = -1;

	if (j.find("playernumber") != j.end()) { //要拿使用者編號 與 真的有傳使用者編號 than
		playernumber = j["playernumber"];
	}

	for (int i = 0; i < getlen; i++) {
		buf.push_back(j[getstri(getname, i + 1)]);
	}

	return playernumber;
}


void mjtransmission::vectortoiarr(std::vector<int> v, int *arr,const int arrlen) {
	for (int i = 0; i < arrlen && i < static_cast<int>(v.size()); i++) {
		arr[i] = v.at(i);
	}
}