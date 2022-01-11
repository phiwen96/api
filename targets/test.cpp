// server.cpp

import Http;
import std;
#include <nlohmann/json.hpp>
using namespace nlohmann;

using std::cout, std::endl, std::string;

auto main(int, char **) -> int
{
	cout << "starting test...\n\n";

	auto user = json {};

	user ["username"] = "phille1234";
	user ["password"] = "kisskorv123";
	// cout << user.dump () << endl;

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
			{"Content-Type", "application/json; charset-UTF-8"},
			{"Content-Length", "20"}
		},

		.data = user.dump ()
	};


	auto request_copy = http_request::parse (to_string (request));

	if (not request_copy.has_value ())
	{
		cout << "error" << endl;
		exit (1);
	}

	auto response = http_response 
	{
		.status_line = 
		{
			.version = 1.0, 
			.status_code = 200,
			.status_phrase = "OK"
		}, 

		.headers = 
		{
			{"Server", "ph"},
			{"Content-Type", "text/html; charset=UTF-8"},
			{"Content-Length", "?"}
		}
	};

	auto response_copy = http_response::parse (to_string (response));

	if (not response_copy.has_value ())
	{
		cout << "error" << endl;
		exit (1);
	}



	// cout << copy.value () << endl;
	// cout << copy << endl;

	cout << "Success!" << endl;

	return 0;
}
