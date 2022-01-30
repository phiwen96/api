module;
#include <string>
export module Usr;

import Core;
// import RemoteClient;

using std::string;

export
{
	template <typename T>
	concept Usr = requires (T usr)
	{
		{usr.name()} -> String;
		{usr.email()} -> String;
		{usr.password()} -> String;

		usr.name() = "Philip";
		usr.email() = "p@w.com";
		usr.name() = "123kg458FS4@";
	};

	struct usr_t 
	{

		// user_t (auto)

	// private:
		string _name;
		// string _
		int _id;
	};

}