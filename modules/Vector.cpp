// module;

// #include <malloc.h>

export module Vector;

// import Size;
// import Same;

// export 
// {
// 	template <typename T>
// 	requires requires {typename T::element_type;}
// 	using element_type = typename T::element_type;

// 	template <typename T>
// 	concept Vector = requires (T t, element_type <T> e, T other)
// 	{
// 		t += e;
// 		t = other;
// 		t += other;
// 		T {t + other};

// 		{length (t)} noexcept -> Size;

// 		{begin (t)} noexcept -> Same <element_type <T>*>;
// 		{end (t)} noexcept -> Same <element_type <T>*>;
// 	};

// 	template <typename T>
// 	struct vector 
// 	{
// 		using element_type = T;

// 		vector (auto&&... t) noexcept : _size {sizeof... (t)}, _max {_size}
// 		{
// 			_begin = (T*) malloc (sizeof (T) * _size);
// 			auto b = _begin;

// 			(([&](T&&)noexcept -> void {
// 				*_begin = (T&&) t;
// 				++_begin; 
// 			}), ...);

// 			_begin = b;
// 		}

// 		operator T* ()
// 		{
// 			return _begin;
// 		}

// 		constexpr friend auto length (vector& me) noexcept -> Size auto 
// 		{
// 			return me._size;
// 		}

// 		constexpr auto operator [] (Size auto i) noexcept -> T&
// 		{
// 			return _begin [i];
// 		}

// 		constexpr auto operator [] (Size auto i) const noexcept -> T const&
// 		{
// 			return _begin [i];
// 		}

// 		friend auto push (vector& me, T&& t) noexcept -> void 
// 		{
// 			if (me._size >= me._max)
// 			{
// 				me._max *= 2;

// 				me.resize (me._max);
// 			}
// 			me._begin [me._size] = (T&&) t;
// 			++me._size;
// 		}

// 		friend auto begin (vector& me) noexcept -> element_type*
// 		{
// 			return me._begin;
// 		}

// 		friend auto end (vector& me) noexcept -> element_type*
// 		{
// 			return me._begin + me._size;
// 		}

// 		friend auto erase (vector& me, element_type* i) noexcept -> void
// 		{
			
// 		}
		
		
// 	private:

// 		auto resize (Size auto n) noexcept -> void
// 		{
// 			_begin = (T*) realloc (_begin, sizeof (T) * n);
// 			_size = n;
// 		}

// 		T* _begin;
// 		size _size;
// 		size _max;
// 	};
// }