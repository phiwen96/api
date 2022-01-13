#include <signal.h>

export module Server;

export import Client;
export import Messenger;
import Common;


export 
{
	template <typename T>
	concept Server = requires ()
	{
		true;
	};

	template <typename T>
	requires Messenger <T>
	struct server 
	{
		server (String auto&& port, T&& messenger);
		
		
		T _messenger;
	};
}




using std::cout, std::endl;

constexpr auto max_data_size = 1024; // max number of bytes we can get at once
constexpr auto backlog = 10;

inline auto sigchld_handler (int s) -> void
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
	errno = saved_errno;
}

// get sockaddr, IPv4 or
inline auto get_in_addr (sockaddr *sa) -> void *
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

inline auto sendall (int sock, char const* buf, int* len) -> int 
{
	int total = 0;
	int bytesleft = *len;
	int n;

	while (total < *len)
	{
		n = send (sock, buf + total, bytesleft, 0);
		if (n == -1)
		{
			break;
		}
		total += n;
		bytesleft -= n;
	}

	*len = total;

	return n==-1?-1:0; // return -1 on failure, 0 on success
}


export inline auto serve (char const* port, auto&& callback) -> int
{
	

	std::cout << "starting restful api" << std::endl;

	int sockfd, new_fd, numbytes; // listen on sock_fd, new connection on new_fd struct addrinfo hints, *servinfo, *p;
	struct addrinfo *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information 
	socklen_t sin_size;
	int yes = 1;
	char client_address [INET6_ADDRSTRLEN];
	int rv;
	char buf [max_data_size];

	auto hints = addrinfo
	{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};

	
	

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

			if ((numbytes = recv (new_fd, buf, max_data_size-1, 0)) == -1) 
			{ 
				perror("recv");
				exit(1); 
			}

			// cout << "received data from client" << endl;

			

			buf [numbytes] = '\0';

			std::string outgoing = callback (buf, client_address);

			int len = outgoing.size ();

			if (sendall(new_fd, outgoing.c_str (), &len) == -1)
			{
				perror("send");
			}

		close(new_fd); // parent doesn't need this
	}

	return 0;
}