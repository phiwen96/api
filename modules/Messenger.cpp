export module Messenger;

export import Core;
export import Client;

/*
	A "Messenger" should be able to process a 
	given "String" from a "Client" and return a "String" 
	which the server can handle and send back to "Client".

	It basically processes every connection the server gets.
*/

export template <typename T>
concept Messenger = requires (T& messenger, char const* in_message, client const& new_client)
{	
	{messenger (in_message, new_client)} -> String; 
};

