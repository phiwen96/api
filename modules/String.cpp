module;
#include <string>
export module String;

export
{
	template <typename T>
	concept String = requires (T t)
	{
		std::string {t};	
	};
}

// 	constexpr auto length (Char auto * s) noexcept -> Size auto 
// 	{
// 		Size auto result = size {0};

// 		for (Char auto* i = s; *i != '\0'; ++i)
// 		{
// 			++result;
// 		}

// 		return result;
// 	}

// 	template <Char T>
// 	auto clone (T const * src) noexcept -> String auto 
// 	{
// 		Size auto len = length (src);

// 		String auto dst = (T*) malloc (sizeof (T) * len);

// 		for (Size auto i = 0; i < len; ++i)
// 		{
// 			dst [i] = src [i];
// 		}

// 		return dst;
// 	}

// 	struct string 
// 	{	

// 		string () noexcept 
// 		{
			
// 		}

// 	private:
// 		char* _begin;
// 		int _size;
// 	};
// }