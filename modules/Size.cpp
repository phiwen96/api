module;
#include <stddef.h>
export module Size;

import Convertible;

export template <typename T>
concept Size = Convertible <T, size_t>;

export using size = size_t;