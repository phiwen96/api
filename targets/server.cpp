// server.cpp



#include <nlohmann/json.hpp>
// #include <signal.h>


// using namespace std;
using namespace nlohmann;

import Server;
// import Darwin;
import Http;
import std;

using std::cout, std::endl, std::string;








auto main (int, char **) -> int
{	
	// read a JSON file filled with users
	auto file_users = std::ifstream {"data/users.json"};
	auto users = json {};
	file_users >> users;
	file_users.close ();

	auto logged = std::vector <std::string> {};

	// process a clients message and return a response
	auto callback = [&] (std::string incoming, std::string client_address) -> std::string
	{
		auto response = http_response 
		{
			.status_line = 
			{
				.version = 1.0
			},

			.headers = 
			{
				{"Server", "ph"}
			}
		};

		auto request = http_request::parse (incoming);

		auto get_user = [&] (std::string const& username) -> json* // returns nullptr if not found, otherwise address to user
		{	
			for (auto& user : users) // loop through all existing users to find client user
			{
				if (user ["username"] == username) // correct username
				{
					return &user;
				} 
			}

			return nullptr;
		};

		auto is_logged = [&] () -> bool // true if client is logged in, if false it fills necessary response
		{
			auto i = std::find (logged.begin(), logged.end(), client_address);

			if (i == logged.end ()) // not logged, fill response and return false
			{
				response.status_line.status_code = 403;
				response.status_line.status_phrase = "Forbidden";

				auto status = json 
				{
					{"success", false},
					{"status code", 6},
					{"status message", "Request denied, client not logged in"}
				};
				
				response.data = status.dump ();

				return false;

			} else 
			{
				return true;
			}
		};

		if (not request) // Error in request, return bad call kind of resonse.
		{
			response.status_line.status_code = 400;
			response.status_line.status_phrase = "Bad Request"; 

			auto status = json 
			{
				{"success", false},
				{"status code", 3},
				{"status message", "Could not interpret the request"}
			};

			response.data = status.dump ();

			return to_string (response);
		} 

		auto const& method = request -> request_line.url;
		
		
		if (method == "/login") // client wants to login
		{
			auto client_info = json::parse (request -> data); // parse clients user data

			auto username_ok = false;
			auto password_ok = false;

			auto* user = get_user (client_info ["username"]);

			if (user)
			{
				username_ok = true;

				if ((*user) ["password"] == client_info ["password"]) // correct password
				{
					password_ok = true;
				} 				
			}

			if (password_ok) // correct username and password
			{
				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";

				auto status = json 
				{
					{"success", true},
					{"status code", 1},
					{"status message", "Success"}
				};

				response.data = status.dump ();

				logged.push_back (client_address);

			} else if (username_ok) // incorrect password
			{
				response.status_line.status_code = 404;
				response.status_line.status_phrase = "Not Found";

				auto status = json 
				{
					{"success", false},
					{"status code", 5},
					{"status message", "Login fail, incorrect password"}
				};

				response.data = status.dump ();

			} else  // incorrect username
			{
				response.status_line.status_code = 404;
				response.status_line.status_phrase = "Not Found";

				auto status = json 
				{
					{"success", false},
					{"status code", 4},
					{"status message", "Login fail, incorrect username"}
				};
				
				response.data = status.dump ();
			}

							// std::cout << "logged:" << logged.size << std::endl;


		} else if (method == "/logout") // client wants to logout
		{
			// check if user is logged in
			if (is_logged ()) // OK
			{	
				logged.erase (i); // remove from logged users

				// prepare response message
				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";

				auto status = json 
				{
					{"success", true},
					{"status code", 1},
					{"status message", "Success"}
				};
				
				response.data = status.dump ();
			} 
		} else if (method == "/register") // client wants to register new user
		{
			if (is_logged ())
			{
				auto client_info = json::parse (request -> data); // parse clients user data

				if (get_user (client_info ["username"])) // username already exists
				{
					response.status_line.status_code = 403;
					response.status_line.status_phrase = "Forbidden"; 

					auto status = json 
					{
						{"success", false},
						{"status code", 7},
						{"status message", "Registration declined, username already exists"}
					};

					response.data = status.dump ();

				} else // add user, logg client address
				{
					users += client_info;

					logged.push_back (client_address);

					response.status_line.status_code = 200;
					response.status_line.status_phrase = "OK"; 

					auto status = json 
					{
						{"success", true},
						{"status code", 1},
						{"status message", "Success"}
					};

					response.data = status.dump ();
				}
			}
		} 

		return to_string (response);
	};

	serve ("8080", callback);
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