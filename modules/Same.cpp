export module Same;

template <typename T, typename U>
struct is_same 
{
	static constexpr auto value = false;
};

template <typename T>
struct is_same <T, T>
{
	static constexpr auto value = true;
};



export template <typename T, typename U>
concept Same = is_same <T, U>::value;