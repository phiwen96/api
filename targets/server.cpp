// server.cpp

import Server;
import Http;

using std::cout, std::endl, std::string;
// using namespace std;



auto main (int, char **) -> int
{
	auto callback = [] (string incoming) -> char const*
	{
		// std::stringstream ss {incoming};
		// std::string word;
		// while (ss >> word)
		// 	cout << word << endl;
		// while (std::getline (ss, word))
		// 	cout << word << endl;
		auto request = http_request::parse (incoming);

		cout << "new message!" << endl;
		cout << incoming << endl;
		return "hej";
	};

	api::serve ("8080", callback);
	return 0;
}

// "HTTP/1.1 200 OK\r\n"
// "Server: ph"
// "Content-Type: text/html; charset=UTF-8\r\n\r\n"
// "Content-Length: 200/r/n"

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