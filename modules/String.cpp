module;
#include <string.h>
#include <malloc.h>
export module String;

import Same;
import Convertible;
export import Char;
export import Size;

// export
// {
// 	template <typename T>
// 	concept String = requires (T t)
// 	{
// 		true;
// 		// {clone (t)} noexcept -> Convertible <T>;
// 		// {length (t)} noexcept -> Size;
// 		// {t [0]} noexcept -> Char;		
// 	};

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