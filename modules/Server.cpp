#include <signal.h>
#include <poll.h>
#include <fcntl.h>

export module Server;

export import Client;
export import Messenger;
export import Common;
export import Caller;
export import Connection;

#define fwd(x) std::forward<decltype(x)>(x)

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
	*/

	template <typename server>
	concept Server = requires(server && s)
	{
		s.start();
		s.stop();

		std::move(s);
	};

	template <typename T>
	struct server
	{
		server(server &&) = default;
		server(server const &) = default;
		server(T &&msg) : _messenger{fwd(msg)}
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

		server(T &&msg, int port) : _messenger{fwd(msg)}
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

			polls.reserve (10);

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
				// wait for server socket to be written to
				if (poll(polls.data(), polls.size(), -1) == -1)
				{
					perror("poll error");
					throw;
				}

				int j = 0;
				for (auto i = polls.begin(); i != polls.end(); ++i)
				{
					if (i->revents & POLLIN)
					{
						if (i->fd == _sockid)
						{
							// get remote socket
							if ((remote.sockid = accept(_sockid, (struct sockaddr *)&remote.addr, &remote.len)) == -1)
							{
								perror("accept error");
								throw;
							}

							polls.push_back(pollfd{.fd = remote.sockid, .events = POLLIN});

							break;

						} else
						{
							numbytes = recv (i->fd, buf, sizeof(buf), 0);

							if (numbytes == -1)
							{
								std::cout << j << std::endl;
								perror("recv error");
								throw;
							}
							// client hung up
							else if (numbytes == 0)
							{
								std::cout << j << std::endl;
								std::cout << "client hung up" << std::endl;

								if (close(i->fd) == -1)
								{
									perror("close error");
									throw;
								}
								polls.erase(i);
							}

							// client sent something
							else
							{
								buf [numbytes] = '\0';

								std::thread 
								{
									[&] // lambda
									{
										std::string&& response = _messenger (connection {remote.sockid}, std::string {buf});
										if (sendall (remote.sockid, response.c_str(), response.size()) == -1)
										{
											perror ("sendall error");
											throw;
										}
									}
								}.detach();
							}
							break;
						}
					}
					++j;
				}
				// std::cout << "lol" << std::endl;
			}

			// auto&& c = caller {_sockid};

			// auto buffer = std::array <>

			// auto received_bytes = recv (c.get_socket (), )
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
		T _messenger;
		sockaddr_in _addrport{
			.sin_family = AF_UNSPEC,
			.sin_port = htons(INADDR_ANY),
			.sin_addr.s_addr = htonl(INADDR_ANY)};
		int _sockid;
		bool _running;
	};

	template <typename T>
	auto make_server(T && msg, int port)->server<T>
	{
		return {std::move(msg), port};
	}
}

using std::cout, std::endl;

constexpr auto max_data_size = 1024; // max number of bytes we can get at once
constexpr auto backlog = 10;

// export inline auto serve(char const *port, auto &&callback) -> int
// {

// 	std::cout << "starting restful api" << std::endl;

// 	int sockfd, new_fd, numbytes; // listen on sock_fd, new connection on new_fd struct addrinfo hints, *servinfo, *p;
// 	struct addrinfo *servinfo, *p;
// 	struct sockaddr_storage their_addr; // connector's address information
// 	socklen_t sin_size;
// 	int yes = 1;
// 	char client_address[INET6_ADDRSTRLEN];
// 	int rv;
// 	char buf[max_data_size];

// 	auto hints = addrinfo{
// 		.ai_family = AF_UNSPEC,
// 		.ai_socktype = SOCK_STREAM,
// 		.ai_flags = AI_PASSIVE};

// 	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
// 	{
// 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
// 		return 1;
// 	}

// 	// loop through all the results and bind to the first we can
// 	for (p = servinfo; p != NULL; p = p->ai_next)
// 	{
// 		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
// 		{
// 			perror("server: socket");
// 			continue;
// 		}
// 		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
// 		{
// 			perror("setsockopt");
// 			exit(1);
// 		}
// 		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
// 		{
// 			close(sockfd);
// 			perror("server: bind");
// 			continue;
// 		}

// 		break;
// 	}

// 	freeaddrinfo(servinfo); // all done with this structure
// 	if (p == NULL)
// 	{
// 		fprintf(stderr, "server: failed to bind\n");
// 		exit(1);
// 	}
// 	if (listen(sockfd, backlog) == -1)
// 	{
// 		perror("listen");
// 		exit(1);
// 	}

// 	printf("server: waiting for connections...\n");

// 	while (1)
// 	{ // main accept() loop
// 		sin_size = sizeof their_addr;
// 		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

// 		if (new_fd == -1)
// 		{
// 			perror("accept");
// 			continue;
// 		}

// 		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), client_address, sizeof client_address);
// 		printf("server: got connection from %s\n", client_address);

// 		if ((numbytes = recv(new_fd, buf, max_data_size - 1, 0)) == -1)
// 		{
// 			perror("recv");
// 			exit(1);
// 		}

// 		// cout << "received data from client" << endl;

// 		buf[numbytes] = '\0';

// 		std::string outgoing = callback(buf, client_address);

// 		int len = outgoing.size();

// 		if (sendall(new_fd, outgoing.c_str(), &len) == -1)
// 		{
// 			perror("send");
// 		}

// 		close(new_fd); // parent doesn't need this
// 	}

// 	return 0;
// }

/*
	Implementations of server class functions
*/
