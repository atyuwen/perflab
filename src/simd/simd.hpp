#pragma once

namespace simd
{
	enum architecture : int
	{
		x86,
		arm,
	};

	enum instruction_set : int
	{
		unknown,
		sse_2,
		sse_3,
		sse_4_1,
		avx,
		avx_2,
		neon,
	};

	template <instruction_set iset>
	struct simd_extension;
}

#define SIMD_COMPOSITE(...) {__VA_ARGS__}

#if defined(_M_IX86) || defined(_M_X64)
#include "sse.hpp"
#include "avx.hpp"

#elif defined(_M_ARM)
#include "neon.hpp"

#else
#error "unknown architecture"
#endif

#include "utils.hpp"

