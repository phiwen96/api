// server.cpp

import Http;
import Email;
import std;
#include <nlohmann/json.hpp>
using namespace nlohmann;

using std::cout, std::endl, std::string;

auto main(int, char **) -> int
{
	send_email ("philip.sve@hotmail.com", "t world");
	return 0;
}

