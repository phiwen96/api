// server.cpp

// #include <stdlib.h>
// import Server;
// import Connection;
// import Connection;

// import Darwin;
// import Messenger;
// import Usr;
// import Server;
// import Http;
// import Connection;
#include <iostream>
#include <stdlib.h>
import Server;

// using std::cout, std::endl, std::move, std::string, std::vector;

// #include <nlohmann/json.hpp>
// using namespace nlohmann;

#define EAT(...)

// template <
// 	typename accept_connection,
// 	typename on_disconnect,
// 	typename incoming_message,
// 	typename send_message>
// struct serv
// {
// 	accept_connection acceptConnection;
// };


int main(int argc, char **argv)
{

	auto addConnection = []<Connection T>(T &&new_connection)
	{
		// push (connected, move (new_connection));
	};

	auto const acceptConnection = []<Connection T>(T &&new_connection) -> bool
	{
		EAT(
			// we can use the operator co_await to wait for something without blocking the currently executing thread
			if (co_await alreadyConnected(new_connection)) {
				return false;
			}

			co_await addConnection(move(new_connection));)
		return true;
	};

	auto onDisconnect = []<Connection T>(T &&disconnect) {

	};

	auto incomingMessage = []<Connection T>(T &&from) {

	};

	auto sendMessage = []<Connection T>(T &&to) {

	};

	auto s = make_server(
		argv[1],
		acceptConnection,
		onDisconnect,
		incomingMessage,
		sendMessage);

	s.start();

	// auto i = nr_of_threads ();

	// if (argc != 2)
	// {
	// 	cout << "usage >> " << "<localPORT>" << endl;
	// 	return 1;
	// }

	// process a clients message and return a response
	// auto&& m = [](connection&& c, string&& msg) -> string
	// {
	// 	cout << "from client: " << msg << endl;
	// 	return "hello from server";
	// };

	// auto&& request = [](connection&& c)
	// {

	// };

	// Server auto s = make_server(move(m), atoi (argv [1]));

	// std::cout << "port >> " << s.port () << std::endl;

	// s.start();

	// auto s = make_server (m);

	// s.start ();

	// auto s = server {std::move (m), "8080"};
	// serve("8080", callback);
	return 0;
}

// "HTTP/1.1 200 OK\r\n"
// "Server: ph"
// "Content-Type: text/html; charset=UTF-8\r\n"
// "Content-Length: 200\r\n\r\n"

// 			   "<!DOCTYPE html>\r\n"
// 			   "<html><head><title>Testing</title></head>\r\n"
// 			   "<body><p>Testing</p></body><html>\r\n";

// GET / HTTP/1.1
// Host: 127.0.0.1:8080
// Upgrade-Insecure-Requests: 1
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.1 Safari/605.1.15
// Accept-Language: en-GB,en;q=0.9
// Accept-Encoding: gzip, deflate
// Connection: keep-alive

/*
auto file_users = std::ifstream{"data/users.json"};
auto users = json{};
file_users >> users;
file_users.close();

auto logged = std::unordered_map <std::string, json*> {};

Messenger auto m = [&](client cl, std::string incoming) -> std::string
	{
		auto response = http_response{
			.status_line =
				{
					.version = 1.0},

			.headers =
				{
					{"Server", "ph"}}};

		auto request = http_request::parse(incoming);

		auto get_user = [&](std::string const &username) -> json * // returns nullptr if not found, otherwise address to user
		{
			for (auto &user : users) // loop through all existing users to find client user
			{
				if (user["username"] == username) // correct username
				{
					return &user;
				}
			}

			return nullptr;
		};

		auto is_logged = [&]() -> auto // returns an iterator to logged client if logged, if false it fills necessary response and returns sentinel
		{
			auto i = logged.find(cl.ip_address());
			// auto i = std::find (logged.begin(), logged.end(), client_address);

			if (i == logged.end()) // not logged, fill response and return false
			{
				response.status_line.status_code = 403;
				response.status_line.status_phrase = "Forbidden";

				auto status = json{
					{"success", false},
					{"status code", 6},
					{"status message", "Request denied, client not logged in"}};

				response.data = status.dump();
			}

			return i;
		};

		if (not request) // Error in request, return bad call kind of resonse.
		{
			response.status_line.status_code = 400;
			response.status_line.status_phrase = "Bad Request";

			auto status = json{
				{"success", false},
				{"status code", 3},
				{"status message", "Could not interpret the request"}};

			response.data = status.dump();

			return to_string(response);
		}

		auto const &method = request->request_line.url;

		if (method == "/login") // client wants to login
		{
			auto client_info = json::parse(request->data); // parse clients user data

			auto username_ok = false;
			auto password_ok = false;

			auto *user = get_user(client_info["username"]);

			if (user)
			{
				username_ok = true;

				if ((*user)["password"] == client_info["password"]) // correct password
				{
					password_ok = true;
				}
			}

			if (password_ok) // correct username and password
			{
				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";

				auto status = json{
					{"success", true},
					{"status code", 1},
					{"status message", "Success"}};

				response.data = status.dump();

				logged[cl.ip_address()] = user;
			}
			else if (username_ok) // incorrect password
			{
				response.status_line.status_code = 404;
				response.status_line.status_phrase = "Not Found";

				auto status = json{
					{"success", false},
					{"status code", 5},
					{"status message", "Login fail, incorrect password"}};

				response.data = status.dump();
			}
			else // incorrect username
			{
				response.status_line.status_code = 404;
				response.status_line.status_phrase = "Not Found";

				auto status = json{
					{"success", false},
					{"status code", 4},
					{"status message", "Login fail, incorrect username"}};

				response.data = status.dump();
			}

			// std::cout << "logged:" << logged.size << std::endl;
		}
		else if (method == "/logout") // client wants to logout
		{
			// check if user is logged in
			if (auto i = is_logged(); i != logged.end()) // OK
			{
				// auto client_info = json::parse (request -> data); // parse clients user data
				// auto* user = get_user (client_info ["username"]);
				logged.erase(i); // remove from logged users

				// prepare response message
				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";

				auto status = json{
					{"success", true},
					{"status code", 1},
					{"status message", "Success"}};

				response.data = status.dump();
			}
		}
		else if (method == "/register") // client wants to register new user
		{
			auto client_info = json::parse(request->data); // parse clients user data

			if (get_user(client_info["username"])) // username already exists
			{
				response.status_line.status_code = 403;
				response.status_line.status_phrase = "Forbidden";

				auto status = json{
					{"success", false},
					{"status code", 7},
					{"status message", "Registration declined, username already exists"}};

				response.data = status.dump();
			}
			else // add user, logg client address
			{
				// users += client_info;
				users.push_back(client_info);

				logged[cl.ip_address()] = &users.back();

				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";

				auto status = json{
					{"success", true},
					{"status code", 1},
					{"status message", "Success"}};

				response.data = status.dump();
			}
		} else if (method == "/delete")
		{
			// check if user is logged in
			if (auto i = is_logged(); i != logged.end()) // OK
			{
				// auto client_info = json::parse (request -> data); // parse clients user data
				// auto* user = get_user (client_info ["username"]);
				auto j = users.begin ();

				for (; j < users.end (); ++j)
				{
					if (*j == *(i -> second))
					{
						break;
					}
				}

				if (j == users.end ()) // user not found
				{
					// prepare response message
					response.status_line.status_code = 404;
					response.status_line.status_phrase = "Not Found";

					auto status = json{
						{"success", true},
						{"status code", 8},
						{"status message", "User not found"}};

					response.data = status.dump();

				} else // user found
				{
					logged.erase(i); // remove from logged users
					users.erase (j);

					// prepare response message
					response.status_line.status_code = 200;
					response.status_line.status_phrase = "OK";

					auto status = json{
						{"success", true},
						{"status code", 1},
						{"status message", "Success"}};

					response.data = status.dump();
				}
			}
		} else if (method == "/list")
		{
			if (is_logged () != logged.end ()) // logged in
			{
				auto && users_view = json {};

				for (auto const& u : users)
				{
					auto && view = json
					{
						{"username", u ["username"]},
						{"name", u ["name"]},
						{"email", u ["email"]}
					};

					users_view.push_back (std::move (view));
				}

				// prepare response message
				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";

				auto status = json
				{
					{"success", true},
					{"status code", 1},
					{"status message", "Success"},
					{"users", std::move (users_view)}
				};

				response.data = status.dump();
			}
		}


		return to_string(response);
	};
*/