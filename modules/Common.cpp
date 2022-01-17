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
}
