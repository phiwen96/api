export module Messenger;

export import Common;
export import Client;

export template <typename T>
concept Messenger = requires (T messenger, std::string in)
{
	messenger (in) -> String;
};