module;
#include <stdlib.h>
#include <utility>
export module Buffer;

// import Pointer;
// import Reference;

export 
{
	template <typename buffer>
	concept Buffer = requires (buffer const b)
	{
		true;
	} and requires (buffer b)
	{
		// {b[10]} -> Reference;
		b.load(10); 
		b.grow_by(10);
		b.reset();
		// {b.next()} -> Pointer;
		// {b.move_data()} -> Pointer;
		// {b.data()} -> Pointer;
	};


	template <typename T>
	struct buffer_t 
	{
		buffer_t (buffer_t&& other) : _data {other._data}, _used_bytes {other._used_bytes}, _unused_bytes {other._unused_bytes}
		{
			other._data = NULL;
		}

		buffer_t (buffer_t const&) = delete;

		buffer_t (int start_size) : _data {(T*) malloc (start_size * sizeof (T))}, _used_bytes {0}, _unused_bytes {start_size}
		{

		}

		~buffer_t ()
		{
			free (_data);
		}

		auto used () const 
		{
			return _used_bytes;
		}

		auto unused () const
		{
			return _unused_bytes;
		}

		auto grow_by (auto n_bytes)
		{
			_unused_bytes += n_bytes;
			_data = (T*) realloc (_data, _unused_bytes);
		}

		auto load (auto n_bytes)
		{
			_used_bytes += n_bytes;
		}

		auto* data ()
		{
			return _data;
		}

		auto*&& move_data ()
		{
			return std::move (_data);
		}

		auto* next ()
		{
			return _data + _used_bytes;
		}

		auto reset ()
		{
			_used_bytes = 0;
		}
		
		operator T* ()
		{
			return _data;
		}

	private:
		T* _data;
		signed long _used_bytes;
		signed long _unused_bytes;
	};
};