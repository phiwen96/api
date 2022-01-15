export module Common;


export import std;
export import Darwin;

// using namespace std;
// using namespace nlohmann;


export 
{
	template <typename T>
	concept String = requires (T a, T&& b)
	{
		std::string {a};
		std::string {std::forward<T&&>(b)};
	};

	#define fwd(x) std::forward<x>(x)
}




