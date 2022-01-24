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
// #include <arpa/inet.h>
#include <netdb.h>
#include <bits/socket.h>
#include <sys/epoll.h>
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

	export template <typename accept_connection,
		typename on_disconnect,
		typename incoming_message,
		typename send_message>
	struct server
	{
		server(server &&) = delete;
		server(server const &) = delete;
		server(
			char const* port,
			accept_connection &acceptConnection,
			on_disconnect &onDisconnect,
			incoming_message &incomingMessage,
			send_message &sendMessage) : acceptConnection{acceptConnection}, onDisconnect{onDisconnect}, incomingMessage{incomingMessage}, sendMessage{sendMessage}
		{
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
				// fcntl(_sockid, F_SETFL, O_NONBLOCK | FASYNC);
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

			
		}

		auto start()
		{
			struct
			{
				sockaddr_storage addr;
				unsigned int len = sizeof(addr);
				int sockid;
				char ip_address[INET6_ADDRSTRLEN];
			} remote;

			// auto events_fd = epoll_create1 (0);

			// if (events_fd == -1)
			// {
			// 	perror ("epoll_create1");
			// 	return;
			// }

			// auto* events = (epoll_event) malloc (sizeof (epoll_event) * 100);

			// ::new (events) epoll_event {.events = EPOLLIN | EPOLLET, .fd = _sockid};

			// if (epoll_ctl (events_fd, EPOLL_CTL_ADD, _sockid, events)) == -1)
			// {
			// 	perror ("epoll_ctl");
			// 	return;
			// }
			
			// std::cout << "success" << std::endl;
		}


	private:
		accept_connection &acceptConnection;
		on_disconnect &onDisconnect;
		incoming_message &incomingMessage;
		send_message &sendMessage;

		struct sockaddr_in 
		{
			
		} detail;

		
		int _sockid;
		bool _running;
	};

	export template <typename accept_connection,typename on_disconnect,typename incoming_message,typename send_message>
	auto make_server(char const* port,accept_connection &acceptConnection,on_disconnect &onDisconnect,incoming_message &incomingMessage,send_message &sendMessage)->auto
	{
		return server<accept_connection, on_disconnect, incoming_message, send_message>{port, acceptConnection, onDisconnect, incomingMessage, sendMessage};
	}

