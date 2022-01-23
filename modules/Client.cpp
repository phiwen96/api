module;
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
export module Client;

export import Core;
export import Connection;

/*
	A "Client" is a host with an ip_address.
	This ip_address may be of interest for
	a "Server" for communication but also
	for saving authenticated hosts for
	future services.
*/
export template <typename T>
concept Client = requires(T const client)
{
	{
		client.ip_address()
		} -> String;
};

export struct client
{
	client(client &&) = delete;
	client(client const &) = delete;
	client(String auto const& remoteIP, int remotePort)
	{
		if ((_sockid = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("socket error");
			throw;
		}

		// sockaddr_in remote{
		// 	.sin_family = AF_UNSPEC,
		// 	.sin_port = htons(remotePort),
		// 	.sin_addr.s_addr = inet_addr(remoteIP)};

		sockaddr_in remote{
			.sin_family = AF_UNSPEC,
			.sin_port = htons(remotePort),
			.sin_addr = inet_addr(remoteIP)};

		if (::connect(_sockid, (struct sockaddr *)&remote, sizeof(remote)) == -1)
		{
			perror("connect error");
			throw;
		}
	}
	~client () noexcept
	{
		if (shutdown (_sockid, SHUT_RDWR) == -1)
		{
			perror ("shutdown error");
		}

		if (close (_sockid) == -1)
		{
			perror ("close error");
		}
	}

	// auto read() -> String auto
	// {
	// 	auto len = getpagesize();
	// 	String auto buf = (char*) malloc (sizeof (char) * getpagesize);

	// 	int numbytes;
	// 	if ((numbytes = recv(_sockid, buf, len - 1, 0)) == -1)
	// 	{
	// 		perror("recv error");
	// 		throw;
	// 	}
	// 	buf[numbytes] = '\0';
	// 	return buf;
	// }
	// void write(String auto&& src)
	// {
	// 	if (sendall(_sockid, src, length (src)) == -1)
	// 	{
	// 		perror("sendall error");
	// 		throw;
	// 	}
	// }

	// returns clients ip address
	auto ip_address() const noexcept -> String auto const &
	{
		return ip_addr;
	}

	char ip_addr[INET6_ADDRSTRLEN];
	int _sockid;
	sockaddr_in _addrport{
		.sin_family = AF_UNSPEC
		// .sin_port = htons (52162),
		// .sin_addr.s_addr = htonl (INADDR_ANY)
	};
};

/*
	make sure client struct adheres to Client interface
*/
static_assert(Client<client>);

inline auto sendall(int sock, char const *buf, int *len) -> int
{
	int total = 0;
	int bytesleft = *len;
	int n;

	while (total < *len)
	{
		n = send(sock, buf + total, bytesleft, 0);
		if (n == -1)
		{
			break;
		}
		total += n;
		bytesleft -= n;
	}

	*len = total;

	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}
