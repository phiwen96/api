export module Reference;

template <typename T>
struct is_reference_t
{
	constexpr static bool value = false;
};

template <typename T>
struct is_reference_t <T&>
{
	constexpr static bool value = true;
};

template <typename T>
constexpr auto is_reference = is_reference_t <T>::value;

export 
{
	template <typename T>
	concept Reference = is_reference <T>;
}