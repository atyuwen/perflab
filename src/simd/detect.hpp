#pragma once

#include "simd.hpp"

#if defined(_M_IX86)
#include <intrin.h>
#elif defined(_M_ARM)
//#include <armintr.h>
#endif

namespace simd
{
	static inline instruction_set detect_instruction_set()
	{
#if defined(_M_IX86)
		int features[4];
		__cpuid(features, 1);
		if (features[2] & (1 << 19)) return sse_4_1;
		if (features[3] & (1 << 26)) return sse_2;
#elif defined(_M_ARM)
		return neon;
#endif
		return unknown;
	}
}
