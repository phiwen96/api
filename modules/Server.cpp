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
export import Connection;

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
		s.start();
		s.stop();
	};

	export template <typename new_connection,
		typename on_disconnect,
		typename incoming_message,
		typename send_message>
	struct server
	{
		server(server &&) = delete;
		server(server const &) = delete;
		server(
			char const* port,
			new_connection &newConnection,
			on_disconnect &onDisconnect,
			incoming_message &incomingMessage,
			send_message &sendMessage) : newConnection{newConnection}, onDisconnect{onDisconnect}, incomingMessage{incomingMessage}, sendMessage{sendMessage}
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

			events.reserve (20);

			auto&& listen_event = epoll_event {};

			listen_event.events = EPOLLIN | EPOLLET;
			listen_event.data.fd = _sockid;

			events.push_back (std::move (listen_event));

			if (epoll_ctl (events_fd, EPOLL_CTL_ADD, _sockid, &events[0]) == -1)
			{
				perror ("epoll_ctl");
				close (events_fd);
				return;
			}
		}

		~server()
		{
			close (events_fd);
		}



		auto start()
		{
			struct
			{
				sockaddr_storage addr;
				char ip_address[INET6_ADDRSTRLEN];
				int sockid;
				int port;
				unsigned int len = sizeof(addr);

			} remote;


			for (;;)
			{
				auto happend_events = epoll_wait (events_fd, events.data(), events.size(), -1);

				for (auto i = 0; i < happend_events; ++i)
				{
					if (events[i].data.fd == _sockid) // new connection
					{
						if ((remote.sockid = accept (_sockid, (struct sockaddr *)&remote.addr, &remote.len)) == -1)
						{
							perror ("accept");
							continue;
						}

						newConnection 
						(
							connection 
							{
								remote.sockid
							}
						);

						// EPOLLIN|EPOLLRDHUP|EPOLLET
					
					} else if (events[i].events & EPOLLRDHUP) // disconnection, peer socket is closed 
					{
						

					} else if (events[i].events & EPOLLIN) // new message
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
		send_message &sendMessage;



		std::vector <epoll_event> events;

		int events_fd;
		int _sockid;
		bool _running;
	};

	export template <typename new_connection,typename on_disconnect,typename incoming_message,typename send_message>
	auto make_server(char const* port,new_connection &newConnection,on_disconnect &onDisconnect,incoming_message &incomingMessage,send_message &sendMessage)->auto
	{
		return server<new_connection, on_disconnect, incoming_message, send_message>{port, newConnection, onDisconnect, incomingMessage, sendMessage};
	}

