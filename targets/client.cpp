#include <iostream>
// #include <stdlib.h>
#include <coroutine>
#include <vector>

// #include <string>
// import Server;
// import NetStream;
import RemoteServer;
import Email;
using std::cout, std::endl, std::move, std::vector;
// #include <nlohmann/json.hpp>
// using namespace nlohmann;

// using std::cout, std::cin, std::endl, std::string;

#define EAT(...)

auto main (int argc, char ** argv) -> int
{	
	email ("philip.sve@hotmail.com", "hello from server");	

	return 0;
	// struct {
	// 	std::string ip_address;
	// 	int port;
	// } remote;

	// remote.ip_address = argv [1];
	// remote.port = atoi (argv [2]);
	if (argc != 3)
	{
		cout << "usage >> " << "<remoteIP> <remotePORT>" << endl;
		return 1;
	}




	auto remoteIP = argv [1];
	auto remotePORT = argv [2];

	auto remote_server = remote_server_t {remoteIP, remotePORT};

	char * message;

	remote_server << "hello from client";
	// remote_server >> message;
















	// remote_server >> message;

	// cout << message << endl;



	// auto stream = clientStream {remotePORT, remoteIP};

	// stream << "hello from client";

	// char* incoming;

	// stream >> incoming;

	// auto&& m = [](connection&& serv)
	// {
	// 	serv.write ("hello from client");
	// 	cout << serv.read () << endl;
	// };

	// Client auto cl = client {remoteIP, remotePORT};


	// cl.write("hello from client");
	// cout << cl.read() << endl;
	// auto conn =  cl.connect (argv[1], atoi(argv[2])); 

	// conn.write ("hello from client");
	// cl.close();
	// cout << "tjo" << endl;

	

	// cout << ""

	// c.call ("hej");



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


/*
std::string inp;

	// cout << send ("127.0.0.1", "8080", "hello world") << endl;
	auto user = nlohmann::json{};

	auto register_user = [&]() -> bool // true if success (user is then logged in)
	{
		cout << "first name >> ";
		cin >> inp;
		auto name = inp;
		name += ' ';
		cout << "last name >> ";
		cin >> inp;
		name += inp;
		user["name"] = name;
		cout << "email >> ";
		cin >> inp;
		user["email"] = inp;

		auto request = http_request{
			.request_line =
				{
					.request_type = "POST",
					.version = 1.0,
					.url = "/register"},

			.headers =
				{
					{"Content-Type", "application/json; charset-UTF-8"}},

			.data = user.dump()};

		auto from_server = send(SERVER_ADDRESS, PORT, to_string(request));

		auto response = http_response::parse(from_server);

		if (not response) // Cannot interpret response from client
		{
			std::cout << "error, cant interpret response from server, starting over..." << std::endl;
			return false;
		}

		auto status_json = nlohmann::json::parse(response->data);

		auto status_code = status_json["status code"].get<int>();

		switch (status_code)
		{
			case 1: // success 
			{
				return true;
			}

			case 7: // username already taken
			{

			}
		}

		if (status_code == 1)
		{
			return true;
		}
		else
		{
			std::cout << "error, cant interpret response from server, starting over..." << std::endl;
			return false;
		}
	};

	auto authenticate = [&]() -> bool // true on success
	{
		std::cout << "username >> ";
		std::cin >> inp;
		user["username"] = inp;

		std::cout << "password >> ";
		std::cin >> inp;
		user["password"] = inp;

		auto request = http_request{
			.request_line =
				{
					.request_type = "POST",
					.version = 1.0,
					.url = "/login"},

			.headers =
				{
					{"Content-Type", "application/json; charset-UTF-8"}},

			.data = user.dump()};

		auto from_server = send(SERVER_ADDRESS, PORT, to_string(request));
		auto response = http_response::parse(from_server);

		if (not response) // Cannot interpret response from client
		{
			std::cout << "error, cant interpret response from server, starting over..." << std::endl;
			return false;
		}

		auto status_json = nlohmann::json::parse(response->data);

		auto status_code = status_json["status code"].get<int>();

		switch (status_code)
		{
		case 1: // success
		{
			break;
		}

		case 4: // incorrect username
		{
			// check if new user
			while (true)
			{
				std::cout << "new user? [y/n] >> ";
				std::cin >> inp;

				if (inp == "y") // register user
				{
					return register_user();
				}
				else if (inp == "n")
				{
					std::cout << "try again, ";
					return false;
				}
				else
				{
					std::cout << "try again, ";
				}
			}

			break;
		}

		case 5: // incorrect password
		{
			std::cout << "incorrect password, ";
			return false;
		}
		}
		return true;
	};

AUTH:

	while (not authenticate())
	{
	
	}

	auto close = false;

	std::cout << "online >> ";

	while (not close)
	{
		std::cin >> inp;

		if (inp == "logout")
		{
			// std::cout << "logging u off" << std::endl;

			auto request = http_request{
				.request_line =
					{
						.request_type = "POST",
						.version = 1.0,
						.url = "/logout"},

				.headers =
					{
						{"Content-Type", "application/json; charset-UTF-8"}},

				.data = user.dump()};

			auto from_server = send(SERVER_ADDRESS, PORT, to_string(request));
			auto response = http_response::parse(from_server);

			if (not response) // Cannot interpret response from client
			{
				std::cout << "error, cant interpret response from server, exiting..." << std::endl;
				exit(1);
			}

			auto status_json = nlohmann::json::parse(response->data);

			auto status_code = status_json["status code"].get<int>();

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

		} else if (inp == "delete")
		{
			auto request = http_request
			{
				.request_line =
				{
					.request_type = "POST",
					.version = 1.0,
					.url = "/delete"
				},

				.headers =
				{
					{"Content-Type", "application/json; charset-UTF-8"}
				},

				.data = user.dump()
			};

			auto from_server = send(SERVER_ADDRESS, PORT, to_string(request));
			auto response = http_response::parse(from_server);

			if (not response) // Cannot interpret response from client
			{
				std::cout << "error, cant interpret response from server, exiting..." << std::endl;
				// exit(1);
				continue;
			}

			auto status_json = nlohmann::json::parse(response->data);

			auto status_code = status_json["status code"].get<int>();

			switch (status_code)
			{
			case 1: // success, user deleted, start over authentication
			{
				goto AUTH;
			}

			default: // we assume everything else is an error
			{
				std::cout << "error, skipping command" << std::endl;
				break;
			}
			}
		} else if (inp == "list")
		{
			auto request = http_request
			{
				.request_line =
				{
					.request_type = "GET",
					.version = 1.0,
					.url = "/list"
				},

				.headers =
				{
					{"Content-Type", "application/json; charset-UTF-8"}
				},

				.data = user.dump()
			};

			auto from_server = send(SERVER_ADDRESS, PORT, to_string(request));
			auto response = http_response::parse(from_server);

			if (not response) // Cannot interpret response from client
			{
				std::cout << "error, cant interpret response from server, exiting..." << std::endl;
				// exit(1);
				continue;
			}

			auto data_json = nlohmann::json::parse(response->data);

			auto status_code = data_json["status code"].get<int>();

			switch (status_code)
			{
			case 1: // success
				std::cout << data_json ["users"].dump (4) << std::endl << "online >> ";
				break;
			
			default:
				std::cout << data_json ["status message"] << std::endl << "online >> ";
				break;
			}

		} else if (auto i = inp.find("search "); i == 0) // starts with 'search '
		{
			std::cout << "searching!" << std::endl;
		} else
		{
			std::cout << "not a valid command, type \"help\" for a list of available commands." << std::endl;
		}
	}
*/