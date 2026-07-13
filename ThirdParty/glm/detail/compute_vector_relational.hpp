#pragma once

//#include "compute_common.hpp"

// Suppress potential conflicts with Windows SDK macros
#ifdef call
#undef call
#endif
#ifdef constant
#undef constant
#endif

#include "setup.hpp"
#include <limits>

namespace glm{
namespace detail
{
	template <typename T>
	struct compute_equal
	{
		GLM_FUNC_QUALIFIER GLM_CONSTEXPR static bool apply(T a, T b)
		{
			return a == b;
		}
	};
}//namespace detail
}//namespace glm
