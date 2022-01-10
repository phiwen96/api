import Client;
#include <nlohmann/json.hpp>
using namespace nlohmann;

using std::cout, std::cin, std::endl, std::string;

import Http;

auto main (int, char **) -> int
{
	string inp;


	// cout << send ("127.0.0.1", "8080", "hello world") << endl;
	auto user = json {};

	auto authenticated = false;

	while (not authenticated)
	{
		cout << "Enter username: ";
		cin >> inp;
		user ["username"] = inp;

		cout << "Enter password: ";
		cin >> inp;
		user ["password"] = inp;

		// auto request = http_request 
		// {
		
		// };

		auto msg = std::string 
		{
			"POST /login HTTP/1.1\r\n"
			"Content-type: application/json; charset-UTF-8\r\n"
			"Content-Length: "
		} 
			+ std::to_string (user.dump().size()) 
			+ "\n\n"
			+ user.dump();

		// cout << msg << endl;
		auto from_server = send ("127.0.0.1", "8080", msg.c_str ());
		cout << from_server << endl;
	}

	




	return 0;
}