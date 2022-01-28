module;

// #include <signal.h>
// #include <poll.h>
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
#include <string>
#include <iostream>
#include <thread>
#include <aio.h>
// #include <experimental/net>
// #include <experimental/netfwd>

export module Server;
import Core;
import RemoteClient;

// import Headers;

// export import Buffer;
using std::cout, std::endl;
// import Remote_client;
// export import Connection;

/*
export import Client;
export import Messenger;
export import Core;
export import Connection;
*/

inline auto is_socket_non_blocking(int sockid) -> bool
{

	return fcntl(sockid, F_GETFL) & O_NONBLOCK;
}

inline auto make_socket_non_blocking(int sockid) -> bool
{
	return fcntl(sockid, F_SETFL, fcntl(sockid, F_GETFL) | O_NONBLOCK) == -1;
}

// A "Server" should be able to listen to a specific port
// and handle connections and communications with clients.
// It doesn't really care for the actual messages sent
// back and forth, it focuses more on the networking fancy stuff.
// Therefore, it must be initialized with a "Messenger" object
// which can take care on the gossip part.
// You should also be able to "start()" and "stop()" a "Server".

// TODO
// * use coroutines
// * use concepts

export template <typename server>
concept Server = requires(server s)
{
	run(s);
};

export template <
	typename remote_client,
	typename new_connection,
	typename on_disconnect,
	typename incoming_message,
	typename buffer_prediction,
	typename buffer_grow_prediction>
struct _server
{
	_server(_server &&) = delete;
	_server(_server const &) = delete;
	_server(
		char const *port,
		new_connection &newConnection,
		on_disconnect &onDisconnect,
		incoming_message &incomingMessage,
		buffer_prediction &bufferPrediction,
		buffer_grow_prediction &bufferGrowPrediction) : newConnection{newConnection}, onDisconnect{onDisconnect}, incomingMessage{incomingMessage}, bufferPrediction{bufferPrediction}, bufferGrowPrediction{bufferGrowPrediction}
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
			if ((_sockid = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1)
			{
				perror("socket");
				continue;
			}
			int yes = 1;
			if (setsockopt(_sockid, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			{
				perror("setsockopt");
				continue;
			}
			fcntl(_sockid, F_SETFL, O_NONBLOCK | FASYNC);
			if (bind(_sockid, i->ai_addr, i->ai_addrlen) == -1)
			{
				close(_sockid);
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

		if (listen(_sockid, 10) == -1)
		{
			perror("listen error");
		}

		auto event = epoll_event{};

		event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
		event.data.fd = _sockid;

		if (epoll_ctl(events_fd, EPOLL_CTL_ADD, _sockid, &event) == -1)
		{
			perror("epoll_ctl");
			close(events_fd);
			return;
		}

		events_size = 1;

		events_max_size = 64;

		events = (epoll_event *)malloc(events_max_size * sizeof(epoll_event));
	}

	~_server()
	{
		close(events_fd);
	}

	auto run() -> void
	{
		struct
		{
			sockaddr_storage addr;
			char ip_address[INET6_ADDRSTRLEN];
			int sockid;
			int port;
			unsigned int len = sizeof(addr);

		} rem;

		for (;;)
		{
			auto happend_events = epoll_wait(events_fd, events, events_size, -1);

			for (auto i = 0; i < happend_events; ++i)
			{
				if (events[i].data.fd == _sockid) // new connection
				{
					if ((rem.sockid = accept(_sockid, (struct sockaddr *)&rem.addr, &rem.len)) == -1)
					{
						perror("accept");
						continue;
					}

					fcntl(rem.sockid, F_SETFL, O_NONBLOCK);

					auto &&event = epoll_event{};

					event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
					event.data.fd = rem.sockid;

					if (events_size >= events_max_size)
					{
						events_max_size *= 2;
						events = (epoll_event *)realloc(events, sizeof(epoll_event) * events_max_size);
					}

					if (epoll_ctl(events_fd, EPOLL_CTL_ADD, rem.sockid, &event) == -1)
					{
						perror("epoll_ctl");
						close(events_fd);
						throw;
					}

					++events_size;

					newConnection(
						remote_client{
							rem.sockid});

					// EPOLLIN|EPOLLRDHUP|EPOLLET
				}
				else if (events[i].events & EPOLLRDHUP) // disconnection, peer socket is closed
				{
					if (epoll_ctl(events_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]) == -1)
					{
						perror("epoll_ctl");
						throw;
					}

					if (close(events[i].data.fd) == -1)
					{
						perror("close");
					}

					onDisconnect(
						remote_client{rem.sockid});
				}
				else if (events[i].events & EPOLLIN) // new message
				{
					// cout << "new message" << endl;

					// handle asynchronously so we can keep up with other connections

					auto r = remote_client_t {events[i].data.fd};
					auto &&buffer = buffer_t<char>{bufferPrediction(r)}; // let front-end decide
					constexpr auto len = 1;
					aiocb const *list[len];

					do
					{
						auto read_feature = make_aio_request(events[i].data.fd, buffer.next(), buffer.unused() - 1);

						auto read_result = aio_read(&read_feature);

						if (read_result == -1)
						{
							perror("aio_read");
							throw;
						}

						list[0] = &read_feature;

						// wait_for(read_future);
						aio_suspend(list, len, NULL);

						auto bytesRead = aio_return(&read_feature);
						// cout << bytesRead << endl;

						if (bytesRead == -1)
						{
							if (errno == EINVAL)
							{
								cout << "EINVAL" << endl;
							}
							else if (errno == ENOSYS)
							{
								cout << "ENOSYS" << endl;
							}
							perror("aio_return");
							break;
						}
						else if (bytesRead == 0)
						{
							break;
						}
						else
						{
							buffer.load(bytesRead);

							if (buffer.unused() <= 1)
							{

								buffer.grow_by(bufferGrowPrediction(buffer.used(), r));
							}
							else
							{
								break;
							}
						}

					} while (true);

					*buffer.next() = '\0';

					incomingMessage(std::move(r), std::move(buffer));
				}

			CONTINUE:
				continue;
			BREAK:
				break;
			}
		}
	}

private:
	new_connection &newConnection;
	on_disconnect &onDisconnect;
	incoming_message &incomingMessage;
	buffer_prediction &bufferPrediction;
	buffer_grow_prediction &bufferGrowPrediction;
	epoll_event *events;
	int events_max_size;
	int events_size;
	int events_fd;
	int _sockid;
	bool _running;

	// runs in new thread
};

export template <typename remote_client, typename new_connection, typename on_disconnect, typename incoming_message, typename buffer_prediction, typename buffer_grow_prediction>
auto make_server(char const *port, new_connection &newConnection, on_disconnect &onDisconnect, incoming_message &incomingMessage, buffer_prediction &bufferPrediction, buffer_grow_prediction &bufferGrowPrediction) -> auto
{
	return _server<remote_client, new_connection, on_disconnect, incoming_message, buffer_prediction, buffer_grow_prediction>{port, newConnection, onDisconnect, incomingMessage, bufferPrediction, bufferGrowPrediction};
}
