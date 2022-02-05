import Http;
import RemoteServer;
import Spotify;
import std;
#include <nlohmann/json.hpp>
using namespace nlohmann;
using std::cout, std::cin, std::endl, std::string;

auto main (int argc, char ** argv) -> int
{
	auto trackID = "6y0igZArWVi6Iz0rj35c1Y";
	
	auto remoteIP = api::spotify::authURL;
	auto remotePORT = api::spotify::PORT;
	cout << remoteIP << endl << remotePORT << endl;
	auto remote = remote_server_t {remoteIP, "80"};

	// auto requestURL = api::spotify::authURL;
	auto requestURL = "https://accounts.spotify.com/api/token";
	
	auto base64 = "Yjc4ZWVkNzBmYWRhNDcxY2FkMDE4MTNkNTUzODQ2NWQ6NDFmNjUxOTM2YjBhNDU1NzhmNGY3NTE1NzZiYWI4YTg=";

	// auto requestURL = string {api::spotify::URL} + "/authorize/?client_id=" + api::spotify::ClientID + "&" trackID;
	auto data = json {};
	data ["grant_type"] = "client_credentials";
	data ["client_id"] = "BQD4DOzwbwi7LHvGDuRqMz74gi9MrZpoKSOppRCeVAJExrCTP9XjOywS6pqMn0MObhEcSuOOE8DRGAmdDvZ9n5-JzOnIxVD_cc3QT7XdACiqYSoGRx9-PFcvhRTmd2L5xX-pBj4Wywy24cmXpT_LX7kQSXmvKoe5Medb";
	data ["client_secret"] = "41f651936b0a45578f4f751576bab8a8";

	auto request = http::request {
			.request_line =
				{
					.request_type = "POST",
					.version = 1.1,
					.url = requestURL},

			.headers =
				{
					{"Content-Type", "application/json; charset-UTF-8"}},

			.data = data.dump()};

	remote << to_string (request);

	// auto response = std::string {};

	// remote >> response;

	// auto parsed = http_repsonse::parse (response);

	// if (not parsed)
	// {
	// 	cout << "could not interpret response from server" << endl;
	// }



	return 0;
}

