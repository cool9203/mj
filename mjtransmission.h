#pragma once
#include <string>
#include "myserversocket.h"
#include "clientsocket.h"
#include "mj.h"
#include "mylib\single_include\nlohmann\json.hpp"
using json = nlohmann::json;


class mjtransmission : public mj {
protected:


public:
	json done();
	std::string getstri(const char *name, int intdata);

	json iarrtojson(std::vector<int> &data, const char *main_json_name, const char *second_json_name);
	json iarrtojson(std::vector<int> &data, const char *main_json_name, const char *second_json_name, bool sendplayernumber, int playernumber);

	int getjsonvalue(json &tj, std::vector<int> &buf, const int getlen, const char *tjname);
	int getjsonvalue(json &tj, std::vector<int> &buf, const int getlen, const char *tjname, const char *getname);

	void vectortoiarr(std::vector<int> v, int *arr,const int arrlen);
};