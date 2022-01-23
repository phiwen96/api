module;
#include <string.h>
export module String;

export import Char;
export import Size;

export
{
	template <typename T>
	concept String = requires (T t)
	{
		{length (t)} noexcept -> Size;
		{t [0]} noexcept -> Char;		
	};

	constexpr auto length (char const* s) noexcept -> Size auto 
	{
		
		for (auto* i = s; *i != '\0'; ++i)
		{

		}
	}


	struct string 
	{	

		string () noexcept 
		{
			
		}

	private:
		char* _begin;
		int _size;
	};
}