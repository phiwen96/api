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
export module Core;

export import Vector;
export import Same;
export import Convertible;
export import String;
export import Char;

export
{
	// get sockaddr, IPv4 or IPv6
	inline auto get_in_addr(sockaddr * sa)->void *
	{
		if (sa->sa_family == AF_INET)
		{
			return &(((struct sockaddr_in *)sa)->sin_addr);
		}

		return &(((struct sockaddr_in6 *)sa)->sin6_addr);
	}

	inline auto sendall(int sock, char const *buf, int len)->int
	{
		int total = 0;
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
}