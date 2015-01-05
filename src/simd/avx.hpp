#pragma once

#include "simd.hpp"
#include <immintrin.h>

namespace simd
{
	// we choose to implement avx2 only 
	// since full 256-bits float and integer instructions are not supported until avx2 (code name "haswell")

	template <>
	struct simd_extension<avx_2>
	{
		enum {arch = x86};
		enum {pack_size = 8};

		struct packed_float
		{
			__m256 data;

			packed_float() {}
			packed_float(const __m256& v) : data(v) {}
			packed_float(const packed_float& v) : data(v.data) {}

			packed_float& operator = (const packed_float& v)
			{
				data = v.data;
				return *this;
			}

			float& operator [] (int lane)
			{
				return data.m256_f32[lane];
			}
		};

		struct packed_int
		{
			__m256i data;

			packed_int() {}
			packed_int(const __m256i& v) : data(v) {}
			packed_int(const packed_int& v) : data(v.data) {}

			packed_int& operator = (const packed_int& v)
			{
				data = v.data;
				return *this;
			}

			int& operator [] (int lane)
			{
				return data.m256i_i32[lane];
			}
		};

		// to be continued...
	};
}
