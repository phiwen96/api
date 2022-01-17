export module Client;

export import Common;


/*
	A "Client" is a host with an ip_address.
	This ip_address may be of interest for 
	a "Server" for communication but also
	for saving authenticated hosts for
	future services.
*/
export template <typename T>
concept Client = requires (T const client)
{
	{client.ip_address ()} -> String;
};

export struct client 
{
	client(client&&) = default;
	client(client const&) = default;
	client (int port)
	{
		_addrport.sin_port = htons (port);

		if ((_sockid = socket (PF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror ("socket error");
			throw;
		}
	}

	auto call (char const* msg)
	{
		if (connect (_sockid, (struct sockaddr*) &_addrport, sizeof (_addrport)) == -1)
		{
			perror ("connect error");
			throw;
		}

		auto sent_bytes = send (_sockid, msg, strlen (msg), 0);

		if (sent_bytes == -1)
		{
			perror ("send error");
			throw;
		}

		
	}
	// returns clients ip address
	auto ip_address () const noexcept -> String auto const&
	{
		return ip_addr;
	}
	
	char ip_addr [INET6_ADDRSTRLEN];
	int _sockid;
	sockaddr_in _addrport
	{
		.sin_family = AF_INET,
		// .sin_port = htons (52162),
		.sin_addr.s_addr = htonl (INADDR_ANY)
	};	
};

/*
	make sure client struct adheres to Client interface
*/
static_assert (Client <client>);

// get sockaddr, IPv4 or IPv6:
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

constexpr auto max_data_size = 1024; // max number of bytes we can get at once
constexpr auto backlog = 10;



export inline auto send(char const *address, char const *port, std::string const& message) -> char const *
{
	int sockfd, numbytes;
	char buf[max_data_size];
	struct addrinfo *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	auto hints = addrinfo{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM};

	if ((rv = getaddrinfo(address, port, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}
	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		exit(1);
	}
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	// printf("client: connecting to %s\n", s);
	freeaddrinfo(servinfo); // all done with this structure

	int len = message.size();

	if (sendall(sockfd, message.c_str (), &len) == -1)
	{
		perror("send");
	}

	if ((numbytes = recv(sockfd, buf, max_data_size - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	// printf("client: received '%s'\n", buf);
	close(sockfd);

	char *response = (char *)std::malloc(sizeof(char) * numbytes);
	strcpy(response, buf);

	return response;
}


