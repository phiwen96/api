export module User;

import Client;

export template <typename T>
concept User = Client <T> and requires (T user)
{
	{user.name()} -> String;
	{user.email()} -> String;
	{user.password()} -> String;

	user.name() = "Philip";
	user.email() = "p@w.com";
	user.name() = "123kg458FS4@";
};