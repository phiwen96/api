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
#include <aio.h>
export module Core;

export import Json;
export import Buffer;
// export import Vector;
// export import Reference;
// export import Pointer;
export import Same;
export import Convertible;
// export import String;
// export import Char;


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

	inline auto sendall(int sock, char const *buf)->int
	{
		int total = 0;
		int len = strlen (buf);
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

	auto make_aio_request (int fd, auto * dst, size_t len) -> auto
	{
		return aiocb
		{
			.aio_fildes = fd,
			.aio_buf = dst,
			.aio_nbytes = len, 
			.aio_offset = 0 
		};
	};

	auto wait_for (auto&... futures) -> auto
	{
		constexpr auto len = 1;
		aiocb const* list [len];
		auto i = 0;
		auto add_to_list = [&](auto& future){list [i] = &future; ++i;};
		((add_to_list (futures)), ...);
		return aio_suspend (list, len, NULL);
	};
}