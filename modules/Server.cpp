#include <signal.h>
#include <poll.h>
#include <fcntl.h>

export module Server;

export import Client;
export import Messenger;
export import Core;
export import Caller;
export import Connection;

#define fwd(x) std::forward<decltype(x)>(x)

auto is_socket_non_blocking(int sockid) -> bool
{

	return fcntl(sockid, F_GETFL) & O_NONBLOCK;
}

auto make_socket_non_blocking(int sockid) -> bool
{
	return fcntl(sockid, F_SETFL, fcntl(sockid, F_GETFL) | O_NONBLOCK) == -1;
}

export
{
	/*
		A "Server" should be able to listen to a specific port
		and handle connections and communications with clients.
		It doesn't really care for the actual messages sent
		back and forth, it focuses more on the networking fancy stuff.
		Therefore, it must be initialized with a "Messenger" object
		which can take care on the gossip part.
		You should also be able to "start()" and "stop()" a "Server".

		TODO
		* use coroutines
		* use concepts
	*/

	template <typename server>
	concept Server = requires(server && s)
	{

		s.start();
		s.stop();

		std::move(s);
	};

	template <
		typename accept_connection,
		typename on_disconnect,
		typename incoming_message,
		typename send_message>
	struct server
	{
		server(server &&) = delete;
		server(server const &) = delete;
		server(
			accept_connection &acceptConnection,
			on_disconnect &onDisconnect,
			incoming_message &incomingMessage,
			send_message &sendMessage) : acceptConnection{acceptConnection}, onDisconnect{onDisconnect}, incomingMessage{incomingMessage}, sendMessage{sendMessage}
		{
			if ((_sockid = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			{
				perror("socket error");
				throw;
			}

			// set socket to non-blocking (for async)
			fcntl(_sockid, F_SETFL, O_NONBLOCK | FASYNC);

			if (bind(_sockid, (struct sockaddr *)&_addrport, sizeof(_addrport)) == -1)
			{
				throw;
				perror("bind error");
			}

			if (listen(_sockid, 5) == -1)
			{
				perror("listen error");
				throw;
			}
		}

		server(
			int port,
			accept_connection &acceptConnection,
			on_disconnect &onDisconnect,
			incoming_message &incomingMessage,
			send_message &sendMessage) : acceptConnection{acceptConnection}, onDisconnect{onDisconnect}, incomingMessage{incomingMessage}, sendMessage{sendMessage}
		{
			_addrport.sin_port = htons(port);

			if ((_sockid = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			{
				perror("socket error");
				throw;
			}

			// set socket to non-blocking (for async)
			fcntl(_sockid, F_SETFL, O_NONBLOCK | FASYNC);

			if (bind(_sockid, (struct sockaddr *)&_addrport, sizeof(_addrport)) == -1)
			{
				throw;
				perror("bind error");
			}

			if (listen(_sockid, 10) == -1)
			{
				perror("listen error");
				throw;
			}
		}

		auto start()
		{
			auto polls = std::vector<pollfd>{
				pollfd{
					.fd = _sockid,
					.events = POLLIN}};

			polls.reserve(10);

			struct
			{
				sockaddr_storage addr;
				unsigned int len = sizeof(addr);
				int sockid;
				char ip_address[INET6_ADDRSTRLEN];
			} remote;

			int len = getpagesize();
			char buf[len];
			int numbytes;

			while (true)
			{
				// std::cout << polls.size() << std::endl;

				// wait for something to happen
				if (poll(polls.data(), polls.size(), -1) == -1)
				{
					perror("poll error");
					throw;
				}

				// new connection
				if (polls[0] == _sockid)
				{
					// accept connection
					if ((remote.sockid = accept(_sockid, (struct sockaddr *)&remote.addr, &remote.len)) == -1)
					{
						perror("accept error");
						throw;
					}

					// keep it ?
					if (acceptConnection(connection{remote.sockid}))
					{
						polls.push_back(pollfd{.fd = remote.sockid, .events = POLLIN}); // monitor socket
					}
					else // disconnect, close socket
					{
						if (close(remote.sockid) == -1)
						{
							perror("close");
						}
					}

					break;
				}

				for (auto i = polls.begin() + 1; i != polls.end(); ++i)
				{
					if (i->revents & POLLIN)
					{
						if ((numbytes = recv(i->fd, buf, sizeof(buf), 0)) == -1)
						{
							perror("recv error");
						}

						// disconnection
						else if (numbytes == 0)
						{
							// std::cout << "client hung up" << std::endl;

							if (close(i->fd) == -1)
							{
								perror("close error");
								throw;
							}
							polls.erase(i);

							onDisconnect ()
						}

						else
						{
							buf[numbytes] = '\0';

							std::thread{
								[&] // lambda
								{
									std::string &&response = incomingMessage(connection{remote.sockid}, std::string{buf});
									if (sendall(remote.sockid, response.c_str(), response.size()) == -1)
									{
										perror("sendall error");
										throw;
									}
								}}
								.detach();
						}
						break;
					}
				}
			}
		}

		auto stop()
		{
			_running = false;
		}

		auto port() const
		{
			struct sockaddr_in inf;
			unsigned int len = sizeof(inf);
			if (getsockname(_sockid, (struct sockaddr *)&inf, &len) == -1)
			{
				perror("getsockname error");
				throw;
			}
			return ntohs(inf.sin_port);
		}

	private:
		accept_connection &acceptConnection;
		on_disconnect &onDisconnect;
		incoming_message &incomingMessage;
		send_message &sendMessage;

		sockaddr_in _addrport{
			.sin_family = AF_UNSPEC,
			.sin_port = htons(INADDR_ANY),
			.sin_addr.s_addr = htonl(INADDR_ANY)};
		int _sockid;
		bool _running;
	};

	template <
		typename accept_connection,
		typename on_disconnect,
		typename incoming_message,
		typename send_message>
	auto make_server(
		int port,
		accept_connection &acceptConnection,
		on_disconnect &onDisconnect,
		incoming_message &incomingMessage,
		send_message &sendMessage)
		->auto
	{

		return server<accept_connection, on_disconnect, incoming_message, send_message>{port, acceptConnection, onDisconnect, incomingMessage, sendMessage};
	}
}
