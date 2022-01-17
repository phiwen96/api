export module Common;

export import std;
export import Darwin;

// using namespace std;
// using namespace nlohmann;

export
{
	template <typename T>
	concept String = requires(T a, T && b)
	{
		std::string{a};
		std::string{std::forward<T &&>(b)};
	};

#define fwd(x) std::forward<x>(x)

	// get sockaddr, IPv4 or IPv6
	inline auto get_in_addr(sockaddr * sa)->void *
	{
		if (sa->sa_family == AF_INET)
		{
			return &(((struct sockaddr_in *)sa)->sin_addr);
		}

		return &(((struct sockaddr_in6 *)sa)->sin6_addr);
	}

	inline auto sendall(int sock, char const* buf, int len)->int
	{
		int total = 0;
		int bytesleft = len;
		int n;

		while (total < len)
		{
			n = send(sock, buf + total, bytesleft, 0);
			if (n == -1)
			{
				break;
			}
			total += n;
			bytesleft -= n;
		}

		// *len = total;

		return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
	}

	inline auto nr_of_threads() noexcept->auto
	{
		return std::thread::hardware_concurrency();
	}
}
