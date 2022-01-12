import Client;
#include <nlohmann/json.hpp>
using namespace nlohmann;

using std::cout, std::cin, std::endl, std::string;

import Http;

#define PORT "8080"
#define SERVER_ADDRESS "127.0.0.1"

auto main (int, char **) -> int
{
	string inp;


	// cout << send ("127.0.0.1", "8080", "hello world") << endl;
	auto user = json {};

	auto authenticated = false;

	while (not authenticated)
	{
		cout << "username >> ";
		cin >> inp;
		user ["username"] = inp;

		cout << "password >> ";
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
			},

			.data = user.dump ()
		};

		auto from_server = send (SERVER_ADDRESS, PORT, to_string (request));
		auto response = http_response::parse (from_server);

		if (not response) // Cannot interpret response from client
		{
			cout << "error, cant interpret response from server, starting over..." << endl;
			continue;
		}

		auto status_json = json::parse (response -> data);

		auto status_code = status_json ["status code"].get <int> ();

		switch (status_code)
		{
			case 1: // success 
			{
				authenticated = true;
				break;
			}

			case 4: // incorrect username
			{
				// check if new user
				cout << "new user? >> ";
				cin >> inp; 
				break;
			}

			case 5: // incorrect password
			{

				break;
			}
		}
	} // authentication

	cout << "logged in >> ";
	
	auto close = false;

	while (not close)
	{
		cin >> inp;

		if (inp == "exit")
		{
			cout << "logging u off" << endl;

			auto request = http_request 
			{
				.request_line = 
				{
					.request_type = "POST",
					.version = 1.0,
					.url = "/logout"
				},

				.headers = 
				{
					{"Content-Type", "application/json; charset-UTF-8"}
				},

				.data = user.dump ()
			};

			auto from_server = send (SERVER_ADDRESS, PORT, to_string (request));
			auto response = http_response::parse (from_server);

			if (not response) // Cannot interpret response from client
			{
				cout << "error, cant interpret response from server, exiting..." << endl;
				exit (1);
			}

			auto status_json = json::parse (response -> data);

			auto status_code = status_json ["status code"].get <int> ();
			
			switch (status_code)
			{
				case 1: // success
				{
					close = true;
				}

				default: // we assume everything else is an error
				{
					cout << "error, skipping command" << endl;
					break;
				}
			}
		}
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