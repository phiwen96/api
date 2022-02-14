#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
// #include <nlohmann/json.hpp>
export module Facebook;
import Common;
import Darwin;
import std;
import Http;

using std::cout, std::endl;
#define APP_ID "3242190256014276"
#define APP_SECRET "33a22708b958d2375d03c49c8b351fcc"
#define URL "graph.facebook.com"
#define PORT "443"
#define CLIENT_TOKEN "ae444363df110430eda67cfbb9bbc183"
#define ID "1649315222077812"

export
{
	inline auto test_facebook()->void
	{
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();

		cout << "yay" << endl;

		auto *ctx = SSL_CTX_new(TLS_client_method());
		if (!ctx)
		{
			perror("SSL_CTX_new");
			return;
		}
		
		cout << "yay" << endl;


		auto sockfd = get_socket(URL, PORT);

		cout << "yay" << endl;
		

		auto *ssl = SSL_new(ctx);
		if (!ssl or !ctx)
		{
			perror("SSL_CTX_new");
			return;
		}
		
		if (!SSL_set_tlsext_host_name(ssl, URL))
		{
			perror("SSL_set_tlsext_host_name");
			return;
		}

		SSL_set_fd(ssl, sockfd);

		if (SSL_connect(ssl) == -1)
		{
			perror("SSL_connect");
			return;
		}

		// printf("SSL/TLS using %s\n", SSL_get_cipher(ssl));

		

		// auto response = http::to_string (http::request{
		// 	{"GET", 1.1, "/oauth/access_token"},
		// 	{{"client_id", APP_ID}, {"client_secret", APP_SECRET}, {"grant_type", "client_credentials"}}});

		// auto data = nlohmann::json {};
		// data ["access_token"] = std::string {APP_ID} + "|" + CLIENT_TOKEN;

		auto response = http::to_string (http::request{
			{"GET", 1.1, std::string {"/"} + ID + "/accounts"},
			{},
			std::string {"{\"access_token\": \""} + APP_ID + "|" + CLIENT_TOKEN + "\"}"
			});


		// cout << http::to_string(response) << endl;
		cout << response << endl;

		// return;
		
		if (SSL_write (ssl, response.c_str(), response.size()) <= 0)
		{
			perror ("SSL_write");
			return;
		}

		
		int numbytes;
		char buf [1024];

		if ((numbytes = SSL_read (ssl, buf, 1023)) <= 0)
		{
			perror ("SSL_write");
			return;
		}

		buf[numbytes] = '\0';

		cout << "buf >> " << buf << endl;

		SSL_shutdown(ssl);
		close(sockfd);
		SSL_free(ssl);
		SSL_CTX_free(ctx);

		// "/oauth/access_token?client_id=" + APP_ID + "&client_secret=" + APP_SECRET + "&grant_type=client_credentials";
	}
}
