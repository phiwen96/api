export module Common;


export import std;
export import Darwin;

// using namespace std;
// using namespace nlohmann;

export template <typename T>
concept String = std::is_convertible_v <T, std::string>;




