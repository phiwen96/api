#include <stdio.h>

#include <stdlib.h>

#include <openssl/bio.h> /* BasicInput/Output streams */

#include <openssl/err.h> /* errors */

#include <openssl/ssl.h> /* core library */
export module Email;

export import RemoteServer;

import std;
import Darwin;

#define BuffSize 1024
using std::cout, std::endl;

constexpr auto EMAIL = "testcase1234w@outlook.com";
constexpr auto PASSWORD = "testcase1234p";
constexpr auto EMAIL_SERVER_ADDRESS = "smtp-mail.outlook.com";
constexpr auto EMAIL_SERVER_PORT = "587";				//"587"; // TCP ports 465 or 587
// constexpr auto EMAIL_SERVER_PORT = "443";

inline auto sendall(int sock, char const *buf) -> int
{
	int total = 0;
	int len = strlen(buf);
	int bytesleft = len;
	int n;

	while (total < len)
	{
		n = send(sock, buf + total, bytesleft, 0);
		if (n == -1)
		{
			break;
		}
		total += n;
		bytesleft -= n;
	}

	// *len = total;

	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

export
{
	auto email(auto to, auto message)->bool
	{
		[]
		{
			SSL_library_init();
			OpenSSL_add_all_algorithms();
			SSL_load_error_strings();
		}(); // initialize openssl library

		auto *ctx = SSL_CTX_new(TLS_client_method());

		if (not ctx)
		{
			perror("SSL_CTX_new");
			return false;
		}

		SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

		if (SSL_CTX_set_default_verify_paths(ctx) != 1)
		{ // trust “whatever our computer trusts”
			perror("SSL_CTX_set_default_verify_paths");
			return false;
		}

		auto hints = addrinfo{
			.ai_family = AF_UNSPEC,
			.ai_socktype = SOCK_STREAM};

		addrinfo *peer_addr;

		if (getaddrinfo(EMAIL_SERVER_ADDRESS, EMAIL_SERVER_PORT, &hints, &peer_addr))
		{
			perror("getaddrinfo");
			return false;
		}

		auto sockid = socket(peer_addr->ai_family, peer_addr->ai_socktype, peer_addr->ai_protocol);

		if (sockid == -1)
		{
			perror("socket");
			return false;
		}

		if (connect(sockid, peer_addr->ai_addr, peer_addr->ai_addrlen) == -1)
		{
			perror("connect");
			return false;
		}

		freeaddrinfo(peer_addr);

		// sendall(sockid, "ehlo [127.0.0.1]\r\n");

		// cout << "yo" << endl;

		char buf [1024];

		int numbytes;

		if ((numbytes = recv (sockid, buf, 1023, 0)) == -1) {
			perror ("recv");
			return false;
		}

		buf [numbytes] = '\0';

		cout << buf << endl;

		sendall(sockid, "ehlo [127.0.0.1]\r\n");

		if ((numbytes = recv (sockid, buf, 1023, 0)) == -1) {
			perror ("recv");
			return false;
		}

		buf [numbytes] = '\0';

		cout << "ehlo:" << buf << endl;

		sendall (sockid, "STARTTLS\r\n");

		if ((numbytes = recv (sockid, buf, 1023, 0)) == -1) {
			perror ("recv");
			return false;
		}

		buf [numbytes] = '\0';

		cout << "STARTTLS:" << buf << endl;

		

		auto *ssl = SSL_new(ctx);

		if (not ssl)
		{
			perror("SSL_new");
			return false;
		}

		if (!SSL_set_tlsext_host_name(ssl, EMAIL_SERVER_ADDRESS))
		{
			fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");
			ERR_print_errors_fp(stderr);
			return 1;
		}

		SSL_set_fd(ssl, sockid);

		if (SSL_connect(ssl) == -1)
		{
			perror("SSL_connect");
			// fprintf(stderr, "SSL_connect() failed.\n");
			// ERR_print_errors_fp(stderr);
			return false;
		}

		SSL_shutdown(ssl);
		close(sockid);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		return true;
	}
}