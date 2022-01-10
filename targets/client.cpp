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

		auto request = http_request 
		{
			.request_line = 
			{
				.request_type = "POST",
				.version = 1.0,
				.url = "/login"
			},

			.headers = 
			{
				{"Content-Type", "application/json; charset-UTF-8"}
			}
		};

		std::stringstream s;
		s << request;
		
		

		// cout << msg << endl;

		auto from_server = send ("127.0.0.1", "8080", s.str().c_str());
		// cout << from_server << endl;
	}

	




	return 0;
}






/*

auto msg = std::string 
		{
			"POST /login HTTP/1.1\r\n"
			"Content-type: application/json; charset-UTF-8\r\n"
			"Content-Length: "
		} 
			+ std::to_string (user.dump().size()) 
			+ "\n\n"
			+ user.dump();
*/