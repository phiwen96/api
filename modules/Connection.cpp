module;
// import std;

// #include <thread>
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
#include <malloc.h>
// #include <iostream>
// #include <string>

export module Connection;

export import Core;


export
{
	template <typename T>
	concept Connection = requires(T t, char const *s)
	{
		t.read(s);
		t.write("hej");
	};

	struct connection
	{
		connection(int sockid) noexcept : _sockid{sockid}
		{

			// connect (_sockid, )
		}
		connection(connection &&) = default;
		connection(connection const &) = default;
		auto remoteIP() const -> String auto
		{
			// return "hej";
			struct
			{
				sockaddr_storage addr;
				char* ip_address;
				int port;
				unsigned int len = sizeof(addr);
			} detail;

			detail.ip_address = (char*) malloc (sizeof (char) * INET6_ADDRSTRLEN + 1);
			detail.ip_address [INET6_ADDRSTRLEN] = '\0';

			if (getpeername(_sockid, (struct sockaddr *)&detail.addr, &detail.len) == -1)
			{
				perror("getpeername error");
				throw;
			}

			// deal with both IPv4 and IPv6:
			if (detail.addr.ss_family == AF_INET)
			{
				struct sockaddr_in *s = (struct sockaddr_in *)&detail.addr;
				detail.port = ntohs(s->sin_port);
				inet_ntop(AF_INET, &s->sin_addr, detail.ip_address, sizeof detail.ip_address);
			}
			else
			{ // AF_INET6
				struct sockaddr_in6 *s = (struct sockaddr_in6 *)&detail.addr;
				detail.port = ntohs(s->sin6_port);
				inet_ntop(AF_INET6, &s->sin6_addr, detail.ip_address, sizeof detail.ip_address);
			}
			// std::cout << detail.ip_address << std::endl;

			return detail.ip_address; // std::move (std::string {detail.ip_address});
		}

		auto read() -> String auto
		{
			auto len = getpagesize();

			String auto buf = (char *)malloc(sizeof(char) * len);

			int numbytes;
			if ((numbytes = recv(_sockid, buf, len - 1, 0)) == -1)
			{
				perror("recv error");
				throw;
			}
			buf[numbytes] = '\0';
			return (char const *)buf;
		}
		void write(char const *src)
		{
			if (sendall(_sockid, src, strlen(src)) == -1)
			{
				perror("sendall error");
				throw;
			}
		}
		// friend auto operator<<(std::ostream &os, connection const &me) -> std::ostream&
		// {
		// 	os << me.remoteIP ();
		// 	return os;
		// }

	private:
		// std::string _ip_address;
		int _sockid;
	};
}
