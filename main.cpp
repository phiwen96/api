import Server;

using std::cout, std::endl, std::string;
// using namespace std;



auto main (int, char **) -> int
{
	auto callback = [] (string incoming) -> char const*
	{
		cout << incoming << endl;
		return "hej";
	};

	api::serve ("8080", callback);
	return 0;
}