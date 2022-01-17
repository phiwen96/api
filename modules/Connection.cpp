export module Connection;

import Common;

export 
{
	template <typename T>
	concept Connection = requires (T t)
	{
		{t.from()} -> String;
		t.close ();
	};
}