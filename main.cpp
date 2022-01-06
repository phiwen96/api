import Server;

import std;
using std::cout, std::endl;

auto main (int, char **) -> int
{
	auto callback = [] (char const* incoming) -> char const*
	{
		cout << incoming << endl;
		return "hej";
	};

	api::serve ("8080", callback);
	return 0;
}