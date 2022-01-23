export module Messenger;

export import Core;
export import Client;
export import Caller;

using namespace std;

/*
	A "Messenger" should be able to process a 
	given "String" from a "Client" and return a "String" 
	which the server can handle and send back to "Client".

	It basically processes every connection the server gets.
*/

export template <typename T>
concept Messenger = requires (T& messenger, string&& in_message, client&& new_client)
{	
	{messenger (std::move (in_message), std::move (new_client))} -> String; 
};

