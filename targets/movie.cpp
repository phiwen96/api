import Movie;
import RemoteServer;

import Darwin;
import std;

#include <openssl/x509.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

auto main(int, char **) -> int
{

	// auto ipAddress = "https://api.themoviedb.org/3/authentication/token/new?api_key=eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiI1MzQxM2Q5MjA2OGE0NGU2NTE5ZDAyNDExNWYxZDFmMCIsInN1YiI6IjYxZDVjYTE3YmIyNjAyMDA1Yjk5MGUxMiIsInNjb3BlcyI6WyJhcGlfcmVhZCJdLCJ2ZXJzaW9uIjoxfQ.0PR3mzAFCJzhD3bV55qnPI5vk0K3hpkt8tKYPTtdijc";
	auto ipAddress = "https://api.themoviedb.org/3/movie/76341";
	// auto ipAddress = "imdb-api.com/en/API/Title/pk_1u3wtf4bb4w4j0zme/tt1832382";
	// auto ipAddress = "http://api.themoviedb.org/3/search/movie?api_key=53413d92068a44e6519d024115f1d1f0&query=Jack+Reacher";
	auto portNumber = "443";
	// ssl initialization.
	SSL_library_init();
	OpenSSL_add_all_algorithms();

	// init context
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	if (!ctx)
	{
		printf("cannot create SSL context!\n");
		return 1;
	}

	addrinfo *servinfo{nullptr};
	int sockid;

	auto hints = addrinfo{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP
		};

	if (auto r = getaddrinfo(ipAddress, portNumber, &hints, &servinfo); r != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		throw;
	}

	addrinfo *i = servinfo;

	// loop through all the results and connect to the first we can
	for (; i != NULL; i = i->ai_next)
	{
		if ((sockid = socket(i->ai_family, i->ai_socktype,
							 i->ai_protocol)) == -1)
		{
			perror("socket");
			continue;
		}

		/*---Add socket to epoll---*/

		if (connect(sockid, i->ai_addr, i->ai_addrlen) == -1)
		{
			perror("connect");

			continue;
		}

		break;

		// fcntl (sockid, F_SETFL, O_NONBLOCK | FASYNC);
	}

	if (i == NULL)
	{
		// std::cout << "client failed to connect" << std::endl;
		perror("client failed to connect");
		throw;
	}

	SSL *ssl = SSL_new(ctx);
    if (!ctx)
    {
        fprintf(stderr, "SSL_new() failed.\n");
        return 1;
    }

	if (!SSL_set_tlsext_host_name(ssl, ipAddress))
    {
        fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");

        return 1;
    }

	SSL_set_fd(ssl, sockid);
    if (SSL_connect(ssl) == -1)
    {
        fprintf(stderr, "SSL_connect() failed.\n");
        return 1;
    }

	// SSL_write (ssl, buffer, strlen(buffer));

	return 0;
}