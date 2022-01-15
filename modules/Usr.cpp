export module Usr;

import Client;

export template <typename T>
concept Usr = Client <T> and requires (T usr)
{
	{usr.name()} -> String;
	{usr.email()} -> String;
	{usr.password()} -> String;

	usr.name() = "Philip";
	usr.email() = "p@w.com";
	usr.name() = "123kg458FS4@";
};