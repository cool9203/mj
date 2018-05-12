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

	json iarrtojson(int *data, int datalen, const char *main_json_name, const char *second_json_name);
	json iarrtojson(int *data, int datalen, const char *main_json_name, const char *second_json_name, bool sendplayernumber, int playernumber);

	int getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname);
	int getjsonvalue(json &tj, int *buf, const int getlen, bool getplayernumber, const char *tjname, const char *getname);
};