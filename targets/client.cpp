import Client;
import Http;
import std;

#include <nlohmann/json.hpp>
using namespace nlohmann;

// using std::cout, std::cin, std::endl, std::string;



#define PORT "8080"
#define SERVER_ADDRESS "127.0.0.1"

auto main (int, char **) -> int
{
	std::string inp;


	// cout << send ("127.0.0.1", "8080", "hello world") << endl;
	auto user = json {};


	auto authenticate = [&] () -> void
	{
		auto authenticated = false;

		while (not authenticated)
		{
			std::cout << "username >> ";
			std::cin >> inp;
			user ["username"] = inp;

			std::cout << "password >> ";
			std::cin >> inp;
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
			// std::cout << from_server << std::endl;
			auto response = http_response::parse (from_server);

			if (not response) // Cannot interpret response from client
			{
				std::cout << "error, cant interpret response from server, starting over..." << std::endl;
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
					std::cout << "new user? >> ";
					std::cin >> inp; 
					break;
				}

				case 5: // incorrect password
				{

					break;
				}
			}
		} // authentication
	};

AUTH:

	authenticate ();

	auto close = false;

	std::cout << "online >> ";


	while (not close)
	{
		std::cin >> inp;

		if (inp == "logout")
		{
			// std::cout << "logging u off" << std::endl;

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
				std::cout << "error, cant interpret response from server, exiting..." << std::endl;
				exit (1);
			}

			auto status_json = json::parse (response -> data);

			auto status_code = status_json ["status code"].get <int> ();
			
			switch (status_code)
			{
				case 1: // success
				{
					goto AUTH;
					// close = true;
				}

				default: // we assume everything else is an error
				{
					std::cout << "error, skipping command" << std::endl;
					break;
				}
			}

			// authenticate ();
			// authenticated = false;

		} else if (auto i = inp.find ("search "); i == 0) // starts with 'search '
		{
			std::cout << "searching!" << std::endl;

		} else 
		{
			std::cout << "not a valid command, type \"help\" for a list of available commands." << std::endl;
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