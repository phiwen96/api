import Usr;
import Server;
import Http;
import std;

using std::cout, std::endl, std::move, std::string, std::vector, std::tuple, std::pair, std::unordered_map;

#include <nlohmann/json.hpp>
using namespace nlohmann;

// every access token is related with a user
auto access_tokens = unordered_map <string, json*> {};

auto users = json{};

// do nothing yet
auto newConnection = [](auto &&remote)
{
	
};
// do nothing yet
auto onDisconnect = [](auto &&remote) {

};
auto resetPassword = [](auto&& remote, auto&& data) {
	// find access token in http request
	auto i = data.find ("access_token");

	// if http request does not contain any access token
	if (i == data.end ()) {
		// send response with error code
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 10}, {"status_message", "Access token needed for operation"}}.dump()};
	}
	// if clients access token match with one we have
	else if (auto j = access_tokens.find(*i); j != access_tokens.end()) {
		// refer the user
		auto& user = *(j->second);
		// change its password
		user["password"] = data["password"];
		// respond
		remote << http::response {{1.1, 200, "OK"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", true}, {"status_code", 1}, {"status_message", "Success"}}.dump()};

	// if clients access does not match with one we have	
	} else {
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 11}, {"status_message", "Invalid access token"}}.dump()};
	}
};
auto registerUser = [](auto &&remote, auto &&data)
{
	// if ()
	// check if username already exists
	for (auto i = users.begin(); i < users.end(); ++i)
	{
		// already exists2
		if ((*i)["username"] == data["username"])
		{
			remote << http::response {{1.1, 409, "Conflict"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 7}, {"status_message", "Registration declined, username already exists"}}.dump()};
			return;
		}
	}

	// give it an id
	auto user_id = users.size ();
	data["id"] = user_id;
	// add it to existing ones
	users.push_back(data);
	// generate an access token for it
	auto access_token = std::to_string(random_int());

	// remember the access_token and it's related user
	access_tokens [access_token] = &users.back();

	data ["success"] = true;
	data ["status_code"] = 1;
	data ["status_message"] = "success";
	data ["access_token"] = access_token;

	// respond with it
	remote << http::response {{1.1, 201, "Created"}, {{"Content-Type", "application/json; charset-UTF-8"}}, data.dump()};

	// remote << http::response{{1.1, 201, "Created"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", true}, {"status_code", 1}, {"status_message", "success"}, {"access_token", access_token}}.dump()};
};
auto getUser = [](auto &&remote, auto&& data) {
	// find access token in http request
	auto i = data.find ("access_token");

	// if http request does not contain any access token
	if (i == data.end ()) {
		// send response with error code
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 10}, {"status_message", "Access token needed for operation"}}.dump()};
	}
	// if clients access token match with one we have
	else if (auto j = access_tokens.find(*i); j != access_tokens.end()) {
		// refer the user
		auto& user = *(j->second);
		// respond
		remote << http::response {{1.1, 200, "OK"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", true}, {"status_code", 1}, {"status_message", "Success"}, {"id", user["id"]}, {"username", user["username"]}, {"email", user["email"]}}.dump()};

	// if clients access does not match with one we have	
	} else {
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 11}, {"status_message", "Invalid access token"}}.dump()};
	}
};
auto listUsers = [](auto &&remote, auto&& data) {
	// find access token in http request
	auto i = data.find ("access_token");

	// if http request does not contain any access token
	if (i == data.end ()) {
		// send response with error code
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 10}, {"status_message", "Access token needed for operation"}}.dump()};
	}
	// if clients access token match with one we have
	else if (auto j = access_tokens.find(*i); j != access_tokens.end()) {
		// refer the user
		auto& user = *(j->second);
		// make a copy of users
		auto&& safe_users = json {{"success", true}, {"status_code", 1}, {"status_message", "Success"}};
		safe_users["users"] = users;
		// auto safe_users = users;
		// cout << users << endl;
		// remove sensitive info for every user such as passwords
		for (auto& u : safe_users["users"]) {
			u.erase ("password");
		}
		// respond
		remote << http::response {{1.1, 200, "OK"}, {{"Content-Type", "application/json; charset-UTF-8"}}, safe_users.dump()};

	// if clients access does not match with one we have	
	} else {
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 11}, {"status_message", "Invalid access token"}}.dump()};
	}
};
auto authenticateUser = [](auto &&remote, auto&& data)
{
	// find the user
	for (auto &user : users)
	{
		// on username found
		if (user["username"] == data["username"])
		{
			// on password matching
			if (user["password"] == data["password"])
			{
				// generate an access token
				auto access_token = std::to_string(random_int());
				// add access token to existing ones
				access_tokens[access_token] = &user;

				// respond with it
				remote << http::response {{1.1, 200, "OK"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", true}, {"status_code", 1}, {"status_message", "success"}, {"access_token", access_token}, {"id", user["id"]}}.dump()};

				// wrong password
			}
			else
			{
				// send response with error code
				remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 5}, {"status_message", "Incorrect password"}}.dump()};
			}
			return;
		}
	}

	// username not found so respond with an error code
	remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 4}, {"status_message", "Incorrect username"}}.dump()};
};
auto unregisterUser = [](auto &&remote, auto&& data) {
	// find access token in http request
	auto i = data.find ("access_token");

	// if http request does not contain any access token
	if (i == data.end ()) {
		// send response with error code
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 10}, {"status_message", "Access token needed for operation"}}.dump()};
	}
	// if clients access token match with one we have
	else if (auto j = access_tokens.find(*i); j != access_tokens.end()) {
		// refer the user
		auto& user = *(j->second);
		// remove access token
		access_tokens.erase(j);
		// remove the user
		if (auto found = users.find(user); found != users.end()) {
			users.erase(found);
			// respond with success code
			remote << http::response {{1.1, 200, "OK"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", true}, {"status_code", 1}, {"status_message", "Success"}}.dump()};
		} else {
			// if user not found, respond with error code
			remote << http::response {{1.1, 404, "Not Found"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 12}, {"status_message", "Cannot remove a user that doesn't exist"}}.dump()};

		}
	// if clients access token does not match with one we have	
	} else {
		remote << http::response {{1.1, 401, "Unauthorized"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 11}, {"status_message", "Invalid access token"}}.dump()};
	}
};

auto mappedFunctions = tuple{
	pair{string{"/register"}, registerUser},
	pair{string{"/get"}, getUser},
	pair{string{"/list"}, listUsers},
	pair{string{"/unregister"}, unregisterUser},
	pair{string{"/authenticate"}, authenticateUser},
	pair{string{"/reset"}, resetPassword}};
auto method = []<typename T, typename... U>(T && input, U &&...params)
{
	auto methodHelper = [&]<typename TupleT, std::size_t... Is>(const TupleT &tp, std::index_sequence<Is...>)
	{
		auto found = false;
		auto maybeCall = [&](auto const &keyValue)
		{
			if (not found)
			{
				auto const &[key, value] = keyValue;
				if (input == key)
				{
					found = true;
					value(std::forward<U>(params)...);
				}
			}
		};
		(maybeCall(std::get<Is>(tp)), ...);
		return found;
	};
	return methodHelper(mappedFunctions, std::make_index_sequence<std::tuple_size_v<decltype(mappedFunctions)>>{});
};
auto incomingMessage = [](auto &&remote, std::string msg)
{
	// cout << msg << endl;

	// try to parse incoming message
	auto parsed = http::request::parse(msg);

	// if there is a parsing error
	if (not parsed)
	{
		remote << http::response {{1.1, 400, "Bad Request"}, {{"Content-Type: ", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", 3}, {"status_message", "Could not interpret the request"}}.dump()};
	}

	// call the right url method if found
	else if (method(parsed.value().request_line.url, std::move (remote), json::parse(parsed.value().data)))
	{
	}

	// if url method not found
	else
	{
		remote << http::response {{1.1, 400, "Bad Request"}, {{"Content-Type", "application/json; charset-UTF-8"}}, json{{"success", false}, {"status_code", "3"}, {"status_message", "Could not interpret the request"}}.dump()};
	}
};
auto main(int argc, char **argv) -> int
{
	auto file_users = std::ifstream{"data/users.json"};
	file_users >> users;
	file_users.close();

	if (argc != 2)
	{
		cout << "usage >> "
			 << "<localPORT>" << endl;
		return 1;
	}

	auto s = make_server(
		argv[1],
		newConnection,
		onDisconnect,
		incomingMessage);

	s.start();

	return 0;
}
