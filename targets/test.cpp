// server.cpp

import Http;
import std;

using std::cout, std::endl, std::string;

auto main(int, char **) -> int
{
	cout << "starting test...\n\n";

	auto request = http_request{
		.request_line =
			{
				.request_type = "POST",
				.version = 1.0,
				.url = "/login"},

		.headers =
			{
				{"Content-Type", "application/json; charset-UTF-8"},
				{"Content-Length", "20"}}};


	auto copy = http_request::parse (to_string (request));

	if (not copy.has_value ())
	{
		cout << "error" << endl;
		exit (1);
	}
	// cout << copy.value () << endl;
	// cout << copy << endl;

	cout << "Success!" << endl;

	return 0;
}
