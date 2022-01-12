// server.cpp


import Http;
#include <nlohmann/json.hpp>

// using std::cout, std::endl, std::string;
// using namespace std;
using namespace nlohmann;

import Server;

auto main (int, char **) -> int
{	
	// read a JSON file filled with users
	auto file_users = std::ifstream {"data/users.json"};
	auto users = json {};
	file_users >> users;
	file_users.close ();

	

	// logged_clients = (std::string*) std::malloc (sizeof (std::string) * logged_clients_reserved);






	// process a clients message and return a response
	auto callback = [&] (std::string incoming, std::string client_address) -> std::string
	{
		// cout << logged_clients_size << "..." << endl;
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

		auto status_json = json 
		{
			{"success", false},
			{"status code", 3},
			{"status message", "Could not interpret the request"}
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

		// auto logged_in = [&] () -> auto // checks if logged in and fills in the required response if not
		// {
		// 	auto i = logged_clients.begin ();

		// 	for (; i != logged_clients.end (); ++i)
		// 	{
		// 		if (i -> first == client_address)
		// 		{
		// 			break;
		// 		}
		// 	}

		// 	return i;
		// };

		if (not request) // Error in request, return bad call kind of resonse.
		{
			response.status_line.status_code = 400;
			response.status_line.status_phrase = "Bad Request"; 

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
				cout << "password ok!" << endl;
				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";

				status_json ["success"] = true;
				status_json ["status code"] = 1;
				status_json ["status message"] = "Success";

				logged_clients.push_back (client_address);
				// if (logged_clients_size >= logged_clients_reserved)
				// {
				// 	logged_clients_reserved *= 2;
				// 	logged_clients = (std::string*) std::realloc (logged_clients, logged_clients_reserved);
				// }

				// logged_clients [logged_clients_size] = client_address;
				// ++ logged_clients_size;

				// remember client for future requests since its logged in (implement timeout?)
				// logged_clients [client_address] = (*user) ["id"];
				// logged_clients.push_back (client_address);
				// cout << logged_clients.size () << endl;

			} else if (username_ok) // incorrect password
			{
				response.status_line.status_code = 404;
				response.status_line.status_phrase = "Not Found";

				status_json ["success"] = false;
				status_json ["status code"] = 5;
				status_json ["status message"] = "Login fail, incorrect password";

			} else  // incorrect username
			{
				response.status_line.status_code = 404;
				response.status_line.status_phrase = "Not Found";
				
				status_json ["success"] = false;
				status_json ["status code"] = 4;
				status_json ["status message"] = "Login fail, incorrect username";
			}

		} else if (method == "/logout") // client wants to logout
		{
			// check if user is logged in
			// cout << "client wants to logout" << endl;
			// cout << "logged users = " << logged_clients_size << endl;

			auto i = std::find (logged_clients.begin (), logged_clients.end (), client_address);

			if (i != logged_clients.end ()) // OK
			{	
				cout << "ok, loggin out" << endl;
				// prepare response message
				response.status_line.status_code = 200;
				response.status_line.status_phrase = "OK";
				status_json ["status message"] = "Success";
				status_json ["status code"] = 1;

				// remove user from logged in users
				logged_clients.erase (i);

			} else // user not even logged in
			{
				cout << "error, loggin out" << endl;
				response.status_line.status_code = 403;
				response.status_line.status_phrase = "Forbidden";
				status_json ["status message"] = "Request denied, client not logged in";
				status_json ["status code"] = 6;

			} 

		} else if (method == "/register") // client wants to register new user
		{
			
		} 
		
		response.data = status_json.dump ();
		// cout << to_string (response) << endl;

				// cout << "authenticated clients:" << logged_clients.size () << endl;

		// cout << "..." << logged_clients_size << endl;
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