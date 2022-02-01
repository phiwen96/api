export module Socket;

import Common;

export 
{
	template <typename T>
	concept Socket = Same_as <int, T>;
}