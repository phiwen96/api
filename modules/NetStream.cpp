module;
#include <string>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <bits/socket.h>
#include <sys/epoll.h>
#include <vector>
#include <utility>
#include <iostream>
#include <array>
export module NetStream;
import Core;
using std::cout, std::endl;
// using std::string;

export
{
	template <typename T>
	concept NetStream = requires(T stream, std::string in_message)
	{
		stream << "hello world";
		stream >> in_message;
	};

	struct serverStream
	{
		serverStream(serverStream const &) = delete;
		serverStream(serverStream &&) = delete;
		serverStream(auto port) : events{nullptr}, events_max_size{64}, events_size{0}
		{
			if ((events_fd = epoll_create1(0)) == -1)
			{
				perror("epoll_create1");
				throw;
			}

			addrinfo *available;

			auto hints = addrinfo{
				.ai_flags = AI_PASSIVE,
				.ai_family = AF_UNSPEC,
				.ai_socktype = SOCK_STREAM};

			if (getaddrinfo(NULL, port, &hints, &available) == -1)
			{
				perror("getaddrinfo");
				throw;
			}

			addrinfo *i;

			for (i = available; i != nullptr; i = i->ai_next)
			{
				if ((sock_fd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1)
				{
					perror("socket");
					continue;
				}

				if (int yes = 1; setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
				{
					perror("setsockopt");
					continue;
				}

				fcntl(sock_fd, F_SETFL, O_NONBLOCK | FASYNC);

				if (bind(sock_fd, i->ai_addr, i->ai_addrlen) == -1)
				{
					close(sock_fd);
					perror("bind");
					continue;
				}

				break;
			}

			freeaddrinfo(available);

			if (i == nullptr)
			{
				fprintf(stderr, "server: failed to bind\n");
				return;
			}

			if (listen(sock_fd, 100) == -1)
			{
				perror("listen error");
			}

			auto &&listen_event = epoll_event{};

			listen_event.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
			listen_event.data.fd = sock_fd;

			events = (epoll_event *)malloc(sizeof(epoll_event) * events_max_size);
			// events = calloc (events_ma)

			// ::new (events) epoll_event {std::move (listen_event)};

			if (epoll_ctl(events_fd, EPOLL_CTL_ADD, sock_fd, &listen_event) == -1)
			{
				perror("epoll_ctl");
				close(events_fd);
				throw;
			}

			++events_size;
		}

		friend auto operator>>(serverStream &me, char *&dstt) -> serverStream &
		{
			struct
			{
				sockaddr_storage addr;
				char ip_address[INET6_ADDRSTRLEN];
				int sockid;
				int port;
				unsigned int len = sizeof(addr);

			} remote;

			struct 
			{
				char* data = (char*) malloc (sizeof (char) * 1024);
				int max_size = 1024;
				int numbytes = 0;
			} buf;

			auto data_read = false;

			while (not data_read)
			{
				auto n = epoll_wait(me.events_fd, me.events, me.events_max_size, -1);

				for (auto i = 0; i < n; ++i)
				{
					if (me.events[i].data.fd == me.sock_fd) // new connection
					{
						cout << "connection" << endl;
						if ((remote.sockid = accept(me.sock_fd, (struct sockaddr *)&remote.addr, &remote.len)) == -1)
						{
							perror("accept");
							throw;
						}

						fcntl(remote.sockid, F_SETFL, O_NONBLOCK);

						auto &&event = epoll_event{};

						event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
						event.data.fd = remote.sockid;

						if (me.events_size >= me.events_max_size)
						{
							me.events_max_size *= 2;
							me.events = (epoll_event *)realloc(me.events, sizeof(epoll_event) * me.events_max_size);
						}

						if (epoll_ctl(me.events_fd, EPOLL_CTL_ADD, remote.sockid, &event) == -1)
						{
							perror("epoll_ctl");
							close(me.events_fd);
							throw;
						}

						++me.events_size;

						// EPOLLIN|EPOLLRDHUP|EPOLLET
					}

					else if (me.events[i].events & EPOLLRDHUP) // disconnection, peer socket is closed
					{
						cout << "disconnection" << endl;

						if (epoll_ctl(me.events_fd, EPOLL_CTL_DEL, me.events[i].data.fd, &me.events[i]) == -1)
						{
							perror("epoll_ctl");
							throw;
						}
					}
					else if (me.events[i].events & EPOLLIN) // new message
					{
						cout << "message" << endl;
						// constexpr auto start_max_size = 1024;
						// char buf[start_max_size];
						// auto max_size = start_max_size;
						// int numbytes = 0;

						// if ((numbytes = recv(me.events[i].data.fd, buf, max_size - 1, 0)) == -1)
						// {
						// 	perror("recv");
						// 	exit(1);
						// }
						while ((buf.numbytes += recv (me.events[i].data.fd, buf.data + buf.numbytes, buf.max_size - buf.numbytes - 1, 0)) == buf.max_size - 1)
						{
							// buffer is maxed out
							buf.max_size *= 2;
							buf.data = (char*) realloc (buf.data, buf.max_size * sizeof (char));
						}

						buf.data[buf.numbytes] = '\0';
						dstt = buf.data;
						data_read = true;
						// dstt = buffer.data;
					}
					else
					{
						cout << "error" << endl;
						throw;
					}
				}
			}

			

			// auto *&dst = (char *&)dstt;
			// dst = (char *)malloc(sizeof(char) * 10);
			// dst[0] = 'h';
			// dst[1] = '\0';
			return me;
		}

	private:
		epoll_event *events;
		int events_size;
		int events_max_size;
		int sock_fd;
		int events_fd;
	};

	struct clientStream
	{
		clientStream(auto port, auto ip_address)
		{
			if ((events_fd = epoll_create1(0)) == -1)
			{
				perror("epoll_create1");
				throw;
			}

			addrinfo *servinfo{nullptr};

			auto hints = addrinfo{
				.ai_family = AF_UNSPEC,
				.ai_socktype = SOCK_STREAM};

			if (auto r = getaddrinfo(ip_address, port, &hints, &servinfo); r != 0)
			{
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
				throw;
			}

			auto *i = servinfo;

			// loop through all the results and connect to the first we can
			for (; i != NULL; i = i->ai_next)
			{
				if ((sockid = socket(i->ai_family, i->ai_socktype,
									 i->ai_protocol)) == -1)
				{
					perror("socket");
					continue;
				}

				if (connect(sockid, i->ai_addr, i->ai_addrlen) == -1)
				{
					close(sockid);
					perror("connect");
					continue;
				}

				fcntl(sockid, F_SETFL, O_NONBLOCK | FASYNC);

				auto event = epoll_event{};
				event.events = EPOLLOUT;
				event.data.fd = sockid;

				epoll_ctl(events_fd, EPOLL_CTL_ADD, sockid, &event);

				/*---Wait for socket connect to complete---*/
				epoll_wait(events_fd, events, 1, -1);

				if (events[0].events & EPOLLOUT)
				{
					// connection successfull
				}
				else
				{
					cout << "error" << endl;
					throw;
				}

				break;
			}

			if (i == NULL)
			{
				std::cout << "client failed to connect" << std::endl;
				throw;
			}
		}

		friend auto operator>>(clientStream &me, char const *&dstt) -> clientStream &
		{
			struct
			{
				char *data = (char *)malloc(1024 * sizeof(char));
				int size;
				int max_size;
			} buffer;

			// auto *buffer = malloc (1024 * sizeof (char));

			// buffer [0] = '\0';

			auto happend_events = epoll_wait(me.events_fd, me.events, 1, -1);

			for (int i = 0; i < happend_events; ++i)
			{
				if (me.events[i].events & EPOLLIN) // data to be read
				{
					int numbytes;

					// while (numbytes = (recv (me.sockid, buffer.data(), buffer.size() - 1, 0)) > 0)
					// {
					// 	dst = (char*) realloc (dst, numbytes * sizeof (char));
					// 	memcpy (dst, )
					// 	/* code */
					// }
				}
			}

			return me;
		}

		friend auto operator<<(clientStream &me, char const *msg) -> clientStream &
		{
			// auto event = epoll_event{};
			// event.events = EPOLLOUT;
			// event.data.fd = me.sockid;

			// epoll_ctl(events_fd, EPOLL_CTL_ADD, sockid, &event);

			/*---Wait for socket connect to complete---*/
			// epoll_wait(me.events_fd, events, 1, -1);
			// sendall (me.sockid, msg);
			send(me.sockid, msg, strlen(msg), 0);
			return me;
		}

	private:
		epoll_event *events;
		int events_fd;
		int sockid;
	};
}