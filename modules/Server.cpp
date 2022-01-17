#include <signal.h>

export module Server;

export import Client;
export import Messenger;
export import Common;
export import Caller;

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
	concept Server = requires(server&& s)
	{
		s.start();
		s.stop();

		std::move (s);
	};


	template <typename T>
		requires Messenger<T>
	struct server
	{
		server(server&&) = default;
		server(server const&) = default;
		server(T&& msg) : _messenger {fwd (msg)}
		{
			if ((_sockid = socket (PF_INET, SOCK_STREAM, 0)) == -1)
			{
				perror ("socket error");
				throw;
			}

			if (bind (_sockid, (struct sockaddr*) &_addrport, sizeof (_addrport)) == -1)
			{
				throw;
				perror ("bind error");
			}

			if (listen (_sockid, 5) == -1)
			{
				perror ("listen error");
				throw;
			}
		}

		

		auto start () 
		{
			struct {
				sockaddr_in addr;
				unsigned int len = sizeof (addr);
				int sockid;
			} call;

			if ((call.sockid = accept (_sockid, (struct sockaddr*) &call.addr, &call.len)) == -1)
			{
				perror ("accept error");
				throw;
			}

			if (getpeername (call.sockid, (struct sockaddr*) &call.addr, &call.len) == -1)
			{
				perror ("getpeername error");
				throw;
			}



			auto c = caller 
			{
				.sockid = call.sockid
			};

			inet_ntop (call.addr.ss_family, get_in_addr((struct sockaddr *)&call.addr), c.ip_address, sizeof (c.ip_address));
		}	

		auto stop ()
		{

		}	

		auto port () const
		{
			struct sockaddr_in inf;
			unsigned int len = sizeof (inf);
			if (getsockname (_sockid, (struct sockaddr*) &inf, &len) == -1)
			{
				perror ("getsockname error");
				throw;
			}
			return ntohs (inf.sin_port);
		}

		

	private:
	// get sockaddr, IPv4 or IPv6
	auto get_in_addr(sockaddr *sa) -> void *
	{
		if (sa->sa_family == AF_INET)
		{
			return &(((struct sockaddr_in *)sa)->sin_addr);
		}

		return &(((struct sockaddr_in6 *)sa)->sin6_addr);
	}


		T _messenger;
		sockaddr_in _addrport
		{
			.sin_family = AF_INET,
			.sin_port = htons (INADDR_ANY),
			.sin_addr.s_addr = htonl (INADDR_ANY)
		};
		int _sockid;
	};

	template <typename T>
	auto make_server (T&& msg) -> Server auto&&
	{
		return std::move (server <T> {fwd (msg)});
	}
}

using std::cout, std::endl;

constexpr auto max_data_size = 1024; // max number of bytes we can get at once
constexpr auto backlog = 10;

// get sockaddr, IPv4 or IPv6
inline auto get_in_addr(sockaddr *sa) -> void *
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

inline auto sendall(int sock, char const *buf, int *len) -> int
{
	int total = 0;
	int bytesleft = *len;
	int n;

	while (total < *len)
	{
		n = send(sock, buf + total, bytesleft, 0);
		if (n == -1)
		{
			break;
		}
		total += n;
		bytesleft -= n;
	}

	*len = total;

	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

export inline auto serve(char const *port, auto &&callback) -> int
{

	std::cout << "starting restful api" << std::endl;

	int sockfd, new_fd, numbytes; // listen on sock_fd, new connection on new_fd struct addrinfo hints, *servinfo, *p;
	struct addrinfo *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes = 1;
	char client_address[INET6_ADDRSTRLEN];
	int rv;
	char buf[max_data_size];

	auto hints = addrinfo{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE};

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure
	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	if (listen(sockfd, backlog) == -1)
	{
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while (1)
	{ // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), client_address, sizeof client_address);
		printf("server: got connection from %s\n", client_address);

		if ((numbytes = recv(new_fd, buf, max_data_size - 1, 0)) == -1)
		{
			perror("recv");
			exit(1);
		}

		// cout << "received data from client" << endl;

		buf[numbytes] = '\0';

		std::string outgoing = callback(buf, client_address);

		int len = outgoing.size();

		if (sendall(new_fd, outgoing.c_str(), &len) == -1)
		{
			perror("send");
		}

		close(new_fd); // parent doesn't need this
	}

	return 0;
}

/*
	Implementations of server class functions
*/
