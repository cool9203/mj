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
json mjtransmission::iarrtojson(int *data, int datalen, const char *main_json_name, const char *second_json_name) {
	return iarrtojson(data, datalen, main_json_name, second_json_name, false, -1);
}


json mjtransmission::iarrtojson(int *data, int datalen, const char *main_json_name, const char *second_json_name, bool sendplayernumber, int playernumber) {
	json j, secj;

	if (sendplayernumber && playernumber) {
		secj["playernumber"] = playernumber;
	}
	else if (sendplayernumber && playernumber <= 0) {
		throw "player_number_error";
	}

	for (int i = 0; i < datalen; i++) {
		secj[getstri(second_json_name, i + 1)] = data[i];
	}
	j[main_json_name] = secj;

	return j;
}


//return playernumber
int mjtransmission::getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname) {
	return getjsonvalue(tj, buf, getlen, getplayernumber, tjname, "card");
}


int mjtransmission::getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname, const char *getname) {
	json j = tj[tjname];
	int playernumber = -1;

	if (getplayernumber && j.find("playernumber") != j.end()) { //要拿使用者編號 與 真的有傳使用者編號 than
		playernumber = j["playernumber"];
	}

	for (int i = 0; i < getlen; i++) {
		buf[i] = j[getstri(getname, i + 1)];
	}

	return playernumber;
}
