export module Pointer;

template <typename T>
struct is_pointer_t
{
	constexpr static bool value = false;
};

template <typename T>
struct is_pointer_t <T*>
{
	constexpr static bool value = true;
};

template <typename T>
constexpr auto is_pointer = is_pointer_t <T>::value;

export 
{
	template <typename T>
	concept Pointer = is_pointer <T>;
}