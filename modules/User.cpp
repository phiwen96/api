export module User;

import Client;

export template <typename T>
concept User = Client <T> and requires (T user)
{
	{user.name ()} -> String;
};