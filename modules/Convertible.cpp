export module Convertible;

export template <typename T, typename U>
concept Convertible = requires (T t, U u)
{
	u = t;
};