// server.cpp
import Server; 
import Common;
// import Common;
// import Darwin;
import Messenger;
import Usr;
import Server;
import Http;
import std;
import Connection;
import API;

using std::cout, std::endl, std::move, std::string, std::vector, std::tuple, std::pair;

#include <nlohmann/json.hpp>
using namespace nlohmann;

auto users = json{};
// auto logged = vector <connection> {};
auto newConnection = [] (auto&& remote)
{
	// cout << "new connection" << endl;
};
auto onDisconnect = [] (auto&& remote)
{

};
auto createUser = [] (auto&& remote, auto const& data) {
	// auto name = data ["name"];
};
auto getUser = [] (auto&& remote, auto const& user_json) {

};
auto listUsers = [] (auto&& remote, auto const& user_json) {

};
auto loginUser = [] (auto&& remote, auto const& data) {
	// helper function to generate an access token
	auto random_int = [min = 1000, max = 9999] {
		// One engine instance per thread
		static thread_local auto engine = std::default_random_engine{std::random_device{}()}; 
		auto dist = std::uniform_int_distribution<>{min, max};
		return dist (engine);
	};
	 
	// loop all users
	for (auto const& user : users) {
		// matching usernames
		if (user ["username"] == data ["username"]) {
			// and matching passwords
			if (user ["password"] == data ["password"]) {
				// generate an access token
				auto access_token = std::to_string (random_int ());

				// respond with an access token
				remote << http::to_string (http::response {{1.1, 200, "OK"}, {}, json{{"success", "true"}, {"status_code", 1}, {"status_message", "success"}, {"access_token", access_token}}.dump()});
			
			// wrong password
			} else {
				// send response with error code
				remote << http::to_string (http::response {{1.1, 401, "Unauthorized"}, {}, json{{"success", "false"}, {"status_code", 5}, {"status_message", "Incorrect password"}}.dump()});
			}
			return;
		}
	}
	
	// username not found so respond with an error code
	remote << http::to_string (http::response {{1.1, 401, "Unauthorized"}, {}, json{{"success", "false"}, {"status_code", 4}, {"status_message", "Incorrect username"}}.dump()});
};
auto deleteUser = [] (auto&& remote, auto const& user_json) {

};
auto updateUser = [] (auto&& remote, auto const& user_json) {

};
auto mappedFunctions = tuple {
	pair{string{"/create"}, createUser}, 
	pair{string{"/get"}, getUser}, 
	pair{string{"/list"}, listUsers}, 
	pair{string{"/delete"}, deleteUser}, 
	pair{string{"/update"}, updateUser},
	pair{string{"/login"}, loginUser}
};
auto method = [] <typename T, typename... U> (T&& input, U&&... params) {
	auto methodHelper = [&]<typename TupleT, std::size_t... Is>(const TupleT& tp, std::index_sequence<Is...>) {
		auto found = false;
		auto maybeCall = [&] (auto const& keyValue) {
			if (not found){
				auto const& [key, value] = keyValue;
				if (input == key) {
					found = true;
					value (std::forward <U> (params)...);
				}
			}	
		};
		(maybeCall(std::get<Is>(tp)), ...);
		return found;
	};
	return methodHelper (mappedFunctions, std::make_index_sequence <std::tuple_size_v <decltype (mappedFunctions)>> {});
};
auto incomingMessage = [] (auto&& remote, std::string msg) {
	// cout << msg << endl;

	// try to parse incoming message
	auto parsed = http::request::parse (msg);

	// if parsing there is a parsing error
	if (not parsed) {
		remote << http::to_string (http::response {{1.1, 400, "Bad Request"}, {{"Content-Type: ", "application/json; charset-UTF-8\r\n"}}, json {{"success", false},{"status code", 3},{"status message", "Could not interpret the request"}}.dump()});
	} 

	// call the right url method if found
	else if (method (parsed.value().request_line.url, remote, json::parse (parsed.value().data))) {
		
	} 
	
	// if url method not found
	else {
		remote << http::response {{1.1, 400, "Bad Request"}, {}, json {{"success", "false"}, {"status_code", "3"}, {"status_message", "Could not interpret the request"}}.dump()};
	}
};
auto main (int argc, char ** argv) -> int {
	auto file_users = std::ifstream{"data/users.json"};
	file_users >> users;
	file_users.close();

	if (argc != 2) {
		cout << "usage >> " << "<localPORT>" << endl;
		return 1;
	}
	
	auto s = make_server (
		argv [1],
		newConnection, 
		onDisconnect, 
		incomingMessage
	);

	s.start ();

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