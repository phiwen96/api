// server.cpp

import Http;
import Email;
import std;
#include <nlohmann/json.hpp>
using namespace nlohmann;

using std::cout, std::endl, std::string;

auto main(int, char **) -> int
{

	cout << "hello world" << endl;
	email ("philip.sve@hotmail.com", "hello world");
	return 0;
}
