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
// #include <iosream>

export module Server;



// import Headers;
import Core;
import Remote;
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
		run (s);
	};

	export template <typename new_connection,
		typename on_disconnect,
		typename incoming_message>
	struct _server
	{
		_server(_server &&) = delete;
		_server(_server const &) = delete;
		_server(
			char const* port,
			new_connection &newConnection,
			on_disconnect &onDisconnect,
			incoming_message &incomingMessage) : newConnection{newConnection}, onDisconnect{onDisconnect}, incomingMessage{incomingMessage}
		{
			if ((events_fd = epoll_create1 (0)) == -1)
			{
				perror ("epoll_create1");
				throw;
			}

			addrinfo* available;

			auto hints = addrinfo 
			{
				.ai_flags = AI_PASSIVE,
				.ai_family = AF_UNSPEC,
				.ai_socktype = SOCK_STREAM
			};

			if (getaddrinfo (NULL, port, &hints, &available) == -1)
			{
				perror ("getaddrinfo");
				throw;
			}

			addrinfo* i;

			for (i = available; i != nullptr; i = i -> ai_next)
			{
				if ((_sockid = socket (i->ai_family, i->ai_socktype, i->ai_protocol)) == -1)
				{
					perror ("socket");
					continue;
				}
				int yes = 1;
				if (setsockopt (_sockid, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1)
				{
					perror ("setsockopt");
					continue;
				}
				fcntl(_sockid, F_SETFL, O_NONBLOCK | FASYNC);
				if (bind (_sockid, i->ai_addr, i->ai_addrlen) == -1)
				{
					close (_sockid);
					perror ("bind");
					continue;
				}

				break;
			}

			freeaddrinfo (available);

			if (i == nullptr)
			{
				fprintf (stderr, "server: failed to bind\n");
				return;
			}

			if (listen (_sockid, 10) == -1)
			{
				perror("listen error");
			}

			auto&& event = epoll_event {};

			event.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
			event.data.fd = _sockid;

			if (epoll_ctl (events_fd, EPOLL_CTL_ADD, _sockid, &event) == -1)
			{
				perror ("epoll_ctl");
				close (events_fd);
				return;
			}

			events_size = 1;

			events_max_size = 64;

			events = (epoll_event*) malloc (events_max_size * sizeof (epoll_event));
		}

		~_server()
		{
			close (events_fd);
		}



		friend auto run (_server& me) -> void
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
				auto happend_events = epoll_wait (me.events_fd, me.events, me.events_size, -1);

				for (auto i = 0; i < happend_events; ++i)
				{
					if (me.events[i].data.fd == me._sockid) // new connection
					{
						if ((rem.sockid = accept (me._sockid, (struct sockaddr *)&rem.addr, &rem.len)) == -1)
						{
							perror ("accept");
							continue;
						}

						fcntl(rem.sockid, F_SETFL, O_NONBLOCK);

						auto &&event = epoll_event{};

						event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
						event.data.fd = rem.sockid;

						if (me.events_size >= me.events_max_size)
						{
							me.events_max_size *= 2;
							me.events = (epoll_event *)realloc(me.events, sizeof(epoll_event) * me.events_max_size);
						}

						if (epoll_ctl(me.events_fd, EPOLL_CTL_ADD, rem.sockid, &event) == -1)
						{
							perror("epoll_ctl");
							close(me.events_fd);
							throw;
						}

						++me.events_size;

						// me.newConnection 
						// (
						// 	remote 
						// 	{
						// 		rem.sockid
						// 	}
						// );

						// EPOLLIN|EPOLLRDHUP|EPOLLET
					
					} else if (me.events[i].events & EPOLLRDHUP) // disconnection, peer socket is closed 
					{
						

					} else if (me.events[i].events & EPOLLIN) // new message
					{

					}
				}

			}

			


			// ::new (events) epoll_event {.events = EPOLLIN | EPOLLET, .fd = _sockid};

			// if (epoll_ctl (events_fd, EPOLL_CTL_ADD, _sockid, events)) == -1)
			// {
			// 	perror ("epoll_ctl");
			// 	return;
			// }
			
			// std::cout << "success" << std::endl;
		}


	private:
		new_connection &newConnection;
		on_disconnect &onDisconnect;
		incoming_message &incomingMessage;
		epoll_event* events;
		int events_max_size;
		int events_size;
		int events_fd;
		int _sockid;
		bool _running;
	};

	export template <typename new_connection,typename on_disconnect,typename incoming_message>
	auto make_server(char const* port,new_connection &newConnection,on_disconnect &onDisconnect,incoming_message &incomingMessage)->auto
	{
		return _server<new_connection, on_disconnect, incoming_message>{port, newConnection, onDisconnect, incomingMessage};
	}

