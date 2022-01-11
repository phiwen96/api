// server.cpp

import Server;
import Http;
#include <nlohmann/json.hpp>

using std::cout, std::endl, std::string;
// using namespace std;
using namespace nlohmann;


auto main (int, char **) -> int
{	
	// read a JSON file
	auto file_users = std::ifstream {"data/users.json"};
	auto users = json {};
	file_users >> users;

	auto callback = [&users] (string incoming) -> std::string
	{
		auto request = http_request::parse (incoming);

		// cout << request.value () << endl;

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

		auto data = json 
		{
			{"success", false},
			{"status code", 3},
			{"status message", "Could not interpret the request"}
		};

		if (not request) // Error in request, return bad call kind of resonse.
		{
			response.status_line.status_code = 400;
			response.status_line.status_phrase = "Bad Request"; 
			
		} else if (request -> request_line.url == "/login")
		{
			auto client_info = json::parse (request -> data);
			auto exists = false;

			for (auto const& user : users) // loop through all users to find client user
			{
				if (user ["username"] == client_info ["username"]) // correct username
				{
					if (user ["password"] == client_info ["password"]) // correct password
					{
						cout << "yay" << endl;
						response.status_line.status_code = 200;
						response.status_line.status_phrase = "OK";

						data ["success"] = true;
						data ["status code"] = 1;
						data ["status message"] = "Success";

						break;
						
					} else // incorrect password
					{
						response.status_line.status_code = 404;
						response.status_line.status_phrase = "Not Found";

						data ["success"] = false;
						data ["status code"] = 5;
						data ["status message"] = "Login fail, incorrect password";
					}

				} else // incorrect username
				{
					response.status_line.status_code = 404;
					response.status_line.status_phrase = "Not Found";
						
					data ["success"] = false;
					data ["status code"] = 4;
					data ["status message"] = "Login fail, incorrect username";
				}
			}
		} 
		
		response.data = data.dump ();
		cout << to_string (response) << endl;

		return to_string (response);
	};

	api::serve ("8080", callback);
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