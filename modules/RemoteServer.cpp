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
export module RemoteServer;
import Core;

using std::cout, std::endl;

#define MAX_EPOLL_EVENTS 64

export 
{
	struct remote_server_t
	{
		remote_server_t(auto ip_address, auto port)
		{
			events_fd = epoll_create (1);
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

				if (int yes = 1; setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
				{
					perror("setsockopt");
					continue;
				}

				

				 /*---Add socket to epoll---*/

				if (connect(sockid, i->ai_addr, i->ai_addrlen) == -1)
				{				
					// perror ("connect");

					continue;
				}

				fcntl (sockid, F_SETFL, O_NONBLOCK | FASYNC);
				
				auto event = epoll_event{};
				event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
				event.data.fd = sockid;

				epoll_ctl(events_fd, EPOLL_CTL_ADD, sockid, &event);

				// if (connection_result != 0)
				// {
				// 	switch (connection_result)
				// 	{
				// 		case EADDRINUSE:
				// 			throw std::runtime_error {"Local address is already in use."};

				// 		case EADDRNOTAVAIL:
				// 			throw std::runtime_error {"(Internet domain sockets) The socket referred to by sockfd had not previously been bound to an address and, upon attempting to bind it to an ephemeral port, it was determined that all port numbers in the ephemeral port range are currently in use.  See the discussion of /proc/sys/net/ipv4/ip_local_port_range in ip(7)."};
						
				// 		case EAFNOSUPPORT:
				// 			throw std::runtime_error {"The passed address didn't have the correct address family in its sa_family field."};

				// 		case EAGAIN:
				// 			throw std::runtime_error {"For nonblocking UNIX domain sockets, the socket is nonblocking, and the connection cannot be completed immediately.  For other socket families, there are insufficient entries in the routing cache."};
						
				// 		case EALREADY:
				// 			throw std::runtime_error {"The socket is nonblocking and a previous connection attempt has not yet been completed."};

				// 		case EBADF:
				// 			throw std::runtime_error {"sockfd is not a valid open file descriptor."};

				// 		case ECONNREFUSED:
				// 			throw std::runtime_error {"A connect() on a stream socket found no one listening on the remote address."};

				// 		case EFAULT:
				// 			throw std::runtime_error {"The socket structure address is outside the user's address space."};
						
				// 		case EINPROGRESS:
				// 			throw std::runtime_error {"The socket is nonblocking and the connection cannot be completed immediately."};

				// 		case EINTR:
				// 			throw std::runtime_error {"The system call was interrupted by a signal that was caught; see signal(7)."};
						
				// 		case EISCONN:
				// 			throw std::runtime_error {"The socket is already connected."};

				// 		case ENETUNREACH:
				// 			throw std::runtime_error {"Network is unreachable."};

				// 		case ENOTSOCK:
				// 			throw std::runtime_error {"The file descriptor sockfd does not refer to a socket."};

				// 		case EPROTOTYPE:
				// 			throw std::runtime_error {"The socket type does not support the requested communications protocol.  This error can occur, for example, on an attempt to connect a UNIX domain datagram socket to a stream socket."};

				// 		case ETIMEDOUT:
				// 			throw std::runtime_error {"Timeout while attempting connection.  The server may be too busy to accept new connections.  Note that for IP sockets the timeout may be very long when syncookies are enabled on the server."};
						
				// 		default:
				// 			perror ("connect");
				// 			throw;

				// 		// case 
				// 	}
				// }

				
				// if (connection_result == EINPROGRESS)
				// {


				// } else if (connection_result == -1)
				// {
				// 	close(sockid);
				// 	perror("connect");
				// 	continue;
				// }
				BREAK_LOOP:
				break;
				CONTINUE_LOOP:
				continue;
			}

			if (i == NULL)
			{
				std::cout << "client failed to connect" << std::endl;
				throw;
			}

			
		}

		friend auto operator>>(remote_server_t &me, char *&dstt) -> remote_server_t &
		{
			// fcntl(me.sockid, F_SETFL, O_NONBLOCK | FASYNC);


			

			struct
			{
				char *data = (char *)malloc(1024 * sizeof(char));
				int numbytes;
				int max_size = 1024;
			} buf;

			// auto *buffer = malloc (1024 * sizeof (char));

			// buffer [0] = '\0';

			auto happend_events = epoll_wait(me.events_fd, me.events, 1, -1);

			for (auto i = 0; i < happend_events; ++i)
			{
				if (me.events[i].events & EPOLLIN) // data to be read
				{
					int numbytes;

					while ((buf.numbytes += recv(me.events[0].data.fd, buf.data + buf.numbytes, buf.max_size - buf.numbytes - 1, 0)) == buf.max_size - 1)
					{
						// buffer is maxed out
						buf.max_size *= 2;
						buf.data = (char *)realloc(buf.data, buf.max_size * sizeof(char));
					}

					buf.data[buf.numbytes] = '\0';
					dstt = buf.data;
					// data_read = true;
				} 
				else if (me.events[i].events & EPOLLRDHUP)
				{
					cout << "what" << endl;
				}
				else
				{
					cout << "error" << endl;
					throw;
				}
			}

			

			return me;
		}

		friend auto operator<<(remote_server_t &me, char const *msg) -> remote_server_t &
		{
			auto event = epoll_event{};
			event.events = EPOLLOUT;
			event.data.fd = me.sockid;

			epoll_ctl(me.events_fd, EPOLL_CTL_ADD, me.sockid, &event);

			auto happend = epoll_wait(me.events_fd, me.events, 1, -1);

			for (auto i = 0; i < happend; ++i)
			{
				if (me.events[i].events & EPOLLOUT)
				{
					sendall(me.sockid, msg);
				}
				else
				{
					cout << "error" << endl;
					throw;
				}
			}

			return me;
		}

	private:
		epoll_event events [MAX_EPOLL_EVENTS];
		int events_fd;
		int sockid;
	};
}