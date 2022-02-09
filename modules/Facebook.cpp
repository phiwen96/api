#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
export module Facebook;
import Common;
import Darwin;
import std;
import Http;
using std::cout, std::endl;

#define APP_ID "3242190256014276"
#define APP_SECRET "33a22708b958d2375d03c49c8b351fcc"
#define URL "https://graph.facebook.com"

export
{
	inline auto test_facebook()->void
	{
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();

		auto *ctx = SSL_CTX_new(TLS_client_method());
		if (!ctx)
		{
			fprintf(stderr, "SSL_CTX_new() failed.\n");
			return;
		}

		auto sockfd = get_socket ("graph.facebook.com", "443");

		

		close (sockfd);



		auto response = http::request {
			{"GET", 1.1, "/oauth/access_token"},
			{{"client_id", APP_ID}, {"client_secret", APP_SECRET}, {"grant_type", "client_credentials"}}
		};



		cout << http::to_string (response) << endl;
		
// "/oauth/access_token?client_id=" + APP_ID + "&client_secret=" + APP_SECRET + "&grant_type=client_credentials";
		
	}
}
