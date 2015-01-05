#pragma once

#include "simd.hpp"
#include <smmintrin.h>
#include "smmintrin_emu.h"

namespace simd
{
	//////////////////////////////////////////////////////////////////////////
	// SSE 2
	template <>
	struct simd_extension<sse_2>
	{
		enum {arch = x86};
		enum {pack_size = 4};

		struct packed_float
		{
			__m128 data;

			packed_float() {}
			packed_float(const __m128& v) : data(v) {}
			packed_float(const packed_float& v) : data(v.data) {}

			packed_float& operator = (const packed_float& v)
			{
				data = v.data;
				return *this;
			}

			float& operator [] (int lane)
			{
				return data.m128_f32[lane];
			}
		};

		struct packed_int
		{
			__m128i data;

			packed_int() {}
			packed_int(const __m128i& v) : data(v) {}
			packed_int(const packed_int& v) : data(v.data) {}

			packed_int& operator = (const packed_int& v)
			{
				data = v.data;
				return *this;
			}

			int& operator [] (int lane)
			{
				return data.m128i_i32[lane];
			}
		};


		//////////////////////////////////////////////////////////////////////////
		// packed float operations
		static packed_float load(const float* aligned_data)
		{
			return _mm_load_ps(aligned_data);
		}

		static packed_float load_unaligned(const float* unaligned_data)
		{
			return _mm_loadu_ps(unaligned_data);
		}

		static void store(float* aligned_address, const packed_float& v)
		{
			return _mm_store_ps(aligned_address, v.data);
		}

		static void stream(float* aligned_address, const packed_float& v)
		{
			return _mm_stream_ps(aligned_address, v.data);
		}

		static packed_float set(float v0, float v1, float v2, float v3)
		{
			return _mm_setr_ps(v0, v1, v2, v3);
		}

		static packed_float splat(float v0)
		{
			return _mm_set1_ps(v0);
		}

		static packed_float splat(float* pv0)
		{
			return _mm_load1_ps(pv0);
		}

		template<int lane>
		static packed_float splat_lane(const packed_float& v)
		{
			return _mm_shuffle_ps(v.data, v.data, _MM_SHUFFLE(lane, lane, lane, lane));
		}

		static packed_float set_zero()
		{
			return _mm_setzero_ps();
		}

		static packed_float set_one()
		{
			return _mm_set1_ps(1.0f);
		}

		static packed_float rcp_approx(const packed_float& v)
		{
			return _mm_rcp_ps(v.data);
		}

		static packed_float rcp(const packed_float &v)
		{
			return _mm_div_ps(_mm_set1_ps(1.0f), v.data);
		}

		static packed_float abs(const packed_float& v)
		{
			return _mm_and_ps(v.data, _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
		}

		static packed_float min(const packed_float& a, const packed_float& b)
		{
			return _mm_min_ps(a.data, b.data);
		}

		static packed_float max(const packed_float& a, const packed_float& b)
		{
			return _mm_max_ps(a.data, b.data);
		}

		static float min_lane(const packed_float& v)
		{
			__m128 t = v.data;
			t = _mm_min_ps(t, _mm_shuffle_ps(t, t, _MM_SHUFFLE(1, 0, 3, 2)));
			t = _mm_min_ps(t, _mm_shuffle_ps(t, t, _MM_SHUFFLE(2, 3, 0, 1)));
			return _mm_cvtss_f32(t);
		}

		static float max_lane(const packed_float& v)
		{
			__m128 t = v.data;
			t = _mm_max_ps(t, _mm_shuffle_ps(t, t, _MM_SHUFFLE(1, 0, 3, 2)));
			t = _mm_max_ps(t, _mm_shuffle_ps(t, t, _MM_SHUFFLE(2, 3, 0, 1)));
			return _mm_cvtss_f32(t);
		}

		// saturate each lane to 16 bits and then pack to a 64-bit integer
		static long long digest(const packed_float& v)
		{
			__m128i t0 = _mm_castps_si128(v.data);
			__m128i t1 = _mm_packs_epi32(t0, t0);
			return t1.m128i_i64[0]; 
		}

		// extract sign bits
		static int movemask(const packed_float& v)
		{
			return _mm_movemask_ps(v.data);
		}

		// return mask.lanes[i].sign_bit ? b.lanes[i] : a.lanes[i]
		static packed_float blend(const packed_float& a, const packed_float& b, const packed_float& mask)
		{
			__m128i t = _mm_cmplt_epi32(_mm_castps_si128(mask.data), _mm_setzero_si128());
			return _mm_blendv_ps_emu(a.data, b.data, _mm_castsi128_ps(t));
		}

		// return mask.bits[i] ? b.bits[i] : a.bits[i]
		static packed_float bitwise_blend(const packed_float& a, const packed_float& b, const packed_float& mask)
		{
			return _mm_blendv_ps_emu(a.data, b.data, mask.data);
		}

		static packed_float add(const packed_float& a, const packed_float& b)
		{
			return _mm_add_ps(a.data, b.data);
		}

		static packed_float sub(const packed_float &a, const packed_float& b)
		{
			return _mm_sub_ps(a.data, b.data);
		}

		static packed_float mul(const packed_float &a, const packed_float& b)
		{
			return _mm_mul_ps(a.data, b.data);
		}

		// return a * b + c
		static packed_float muladd(const packed_float& a, const packed_float& b, const packed_float& c)
		{
			return _mm_add_ps(_mm_mul_ps(a.data, b.data), c.data);
		}

		static packed_float div(const packed_float &a, const packed_float& b)
		{
			return _mm_div_ps(a.data, b.data);
		}

		static packed_float bitwise_and(const packed_float &a, const packed_float& b)
		{
			return _mm_and_ps(a.data, b.data);
		}

		static packed_float bitwise_or(const packed_float &a, const packed_float& b)
		{
			return _mm_or_ps(a.data, b.data);
		}

		static packed_float bitwise_xor(const packed_float &a, const packed_float& b)
		{
			return _mm_xor_ps(a.data, b.data);
		}

		static packed_float bitwise_not(const packed_float &a) SIMD_COMPOSITE
		(
			return bitwise_andnot(a, cast_float(set_ones()));
		)

		// return (~a) & b
		static packed_float bitwise_andnot(const packed_float &a, const packed_float& b)
		{
			return _mm_andnot_ps(a.data, b.data);
		}

		static packed_float is_greater(const packed_float &a, const packed_float& b)
		{
			return _mm_cmpgt_ps(a.data, b.data);
		}

		static packed_float is_greater_equal(const packed_float &a, const packed_float& b)
		{
			return _mm_cmpge_ps(a.data, b.data);
		}

		static packed_float is_less(const packed_float &a, const packed_float& b)
		{
			return _mm_cmplt_ps(a.data, b.data);
		}

		static packed_float is_less_equal(const packed_float &a, const packed_float& b)
		{
			return _mm_cmple_ps(a.data, b.data);
		}

		static packed_float is_equal(const packed_float &a, const packed_float& b)
		{
			return _mm_cmpeq_ps(a.data, b.data);
		}

		static packed_float merge_low(const packed_float& a, const packed_float &b)
		{
			return _mm_shuffle_ps(a.data, b.data, _MM_SHUFFLE(1, 0, 1, 0));
		}

		static packed_float merge_high(const packed_float& a, const packed_float &b)
		{
			return _mm_shuffle_ps(a.data, b.data, _MM_SHUFFLE(3, 2, 3, 2));
		}

		static packed_float merge_odd(const packed_float& a, const packed_float &b)
		{
			return _mm_shuffle_ps(a.data, b.data, _MM_SHUFFLE(2, 0, 2, 0));
		}

		static packed_float merge_even(const packed_float& a, const packed_float &b)
		{
			return _mm_shuffle_ps(a.data, b.data, _MM_SHUFFLE(3, 1, 3, 1));
		}

		static void transpose(packed_float& r0, packed_float& r1, packed_float& r2, packed_float &r3)
		{
			_MM_TRANSPOSE4_PS(r0.data, r1.data, r2.data, r3.data);
		}

		//////////////////////////////////////////////////////////////////////////
		// packed int operations
		static packed_int load(const int* aligned_data)
		{
			return _mm_load_si128((__m128i*)aligned_data);
		}

		static packed_int load_unaligned(const int* unaligned_data)
		{
			return _mm_loadu_si128((__m128i*)unaligned_data);
		}

		static void store(int* aligned_address, const packed_int& v)
		{
			return _mm_store_si128((__m128i*)aligned_address, v.data);
		}

		static void stream(int* aligned_address, const packed_int& v)
		{
			return _mm_stream_si128((__m128i*)aligned_address, v.data);
		}

		static packed_int set(int v0, int v1, int v2, int v3)
		{
			return _mm_setr_epi32(v0, v1, v2, v3);
		}

		static packed_int splat(int v0)
		{
			return _mm_set1_epi32(v0);
		}

		static packed_int splat(int* pv0)
		{
			return _mm_set1_epi32(*pv0);
		}

		template<int lane>
		static packed_int splat_lane(const packed_int& v)
		{
			return _mm_shuffle_epi32(v.data, _MM_SHUFFLE(lane, lane, lane, lane));
		}

		static packed_int set_zeros()
		{
			return _mm_setzero_si128();
		}

		static packed_int set_ones()
		{
			packed_int t;
			return _mm_cmpeq_epi32(t.data, t.data);
		}

		static packed_int abs(const packed_int& v)
		{
			return _mm_abs_epi32_emu(v.data);
		}

		static packed_int min(const packed_int& a, const packed_int& b)
		{
			return _mm_min_epi32_emu(a.data, b.data);
		}

		static packed_int max(const packed_int& a, const packed_int& b)
		{
			return _mm_max_epi32_emu(a.data, b.data);
		}

		static int min_lane(const packed_int& v) SIMD_COMPOSITE
		(
			__m128i t = v.data;
			t = _mm_min_epi32_emu(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(1, 0, 3, 2)));
			t = _mm_min_epi32_emu(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(2, 3, 0, 1)));
			return _mm_cvtsi128_si32(t);
		)

		static int max_lane(const packed_int& v) SIMD_COMPOSITE
		(
			__m128i t = v.data;
			t = _mm_max_epi32_emu(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(1, 0, 3, 2)));
			t = _mm_max_epi32_emu(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(2, 3, 0, 1)));
			return _mm_cvtsi128_si32(t);
		)

		// saturate each lane to 16 bits and then packs to a 64-bit integer
		static long long digest(const packed_int& v)
		{
			__m128i t = _mm_packs_epi32(v.data, v.data);
			return t.m128i_i64[0]; 
		}

		// extract sign bits
		static int movemask(const packed_int& v)
		{
			return _mm_movemask_ps(_mm_castsi128_ps(v.data));
		}

		// return mask.lanes[i].sign_bit ? b.lanes[i] : a.lanes[i]
		static packed_int blend(const packed_int& a, const packed_int& b, const packed_int& mask)
		{
			__m128i t = _mm_cmplt_epi32(mask.data, _mm_setzero_si128());
			return _mm_blendv_epi8_emu(a.data, b.data, t);
		}

		// return mask.bits[i] ? b.bits[i] : a.bits[i]
		static packed_int bitwise_blend(const packed_int& a, const packed_int& b, const packed_int& mask)
		{
			return _mm_blendv_epi8_emu(a.data, b.data, mask.data);
		}

		static packed_int add(const packed_int& a, const packed_int& b)
		{
			return _mm_add_epi32(a.data, b.data);
		}

		static packed_int sub(const packed_int &a, const packed_int& b)
		{
			return _mm_sub_epi32(a.data, b.data);
		}

		static packed_int mul(const packed_int &a, const packed_int& b)
		{
			return _mm_mullo_epi32_emu(a.data, b.data);
		}

		// return a * b + c
		static packed_int muladd(const packed_int& a, const packed_int& b, const packed_int& c)
		{
			return _mm_add_epi32(_mm_mullo_epi32_emu(a.data, b.data), c.data);
		}

		static packed_int bitwise_and(const packed_int &a, const packed_int& b)
		{
			return _mm_and_si128(a.data, b.data);
		}

		static packed_int bitwise_or(const packed_int &a, const packed_int& b)
		{
			return _mm_or_si128(a.data, b.data);
		}

		static packed_int bitwise_xor(const packed_int &a, const packed_int& b)
		{
			return _mm_xor_si128(a.data, b.data);
		}

		static packed_int bitwise_not(const packed_int &a) SIMD_COMPOSITE
		(
			return bitwise_andnot(a, set_ones());
		)

		// return (~a) & b
		static packed_int bitwise_andnot(const packed_int &a, const packed_int& b)
		{
			return _mm_andnot_si128(a.data, b.data);
		}

		static packed_int is_greater(const packed_int &a, const packed_int& b)
		{
			return _mm_cmpgt_epi32(a.data, b.data);
		}

		static packed_int is_greater_equal(const packed_int &a, const packed_int& b) SIMD_COMPOSITE
		(
			return bitwise_or(is_greater(a, b), is_equal(a, b));
		)

		static packed_int is_less(const packed_int &a, const packed_int& b)
		{
			return _mm_cmplt_epi32(a.data, b.data);
		}

		static packed_int is_less_equal(const packed_int &a, const packed_int& b) SIMD_COMPOSITE
		(
			return bitwise_or(is_less(a, b), is_equal(a, b));
		)

		static packed_int is_equal(const packed_int &a, const packed_int& b)
		{
			return _mm_cmpeq_epi32(a.data, b.data);
		}

		template<int n>
		static packed_int shift_right(const packed_int &a)
		{
			return _mm_srai_epi32(a.data, n);
		}

		template<int n>
		static packed_int shift_left(const packed_int &a)
		{
			return _mm_slli_epi32(a.data, n);
		}

		static int allzeros(const packed_int& v) SIMD_COMPOSITE
		(
			__m128i t = _mm_packs_epi32(v.data, v.data);
			return t.m128i_u64[0] == 0;
		)

		static int allzeros(const packed_int& v, const packed_int& mask)
		{
			return _mm_testz_si128_emu(v.data, mask.data);
		}

		//////////////////////////////////////////////////////////////////////////
		// packed int / float converting/casting
		static packed_int convert_int(const packed_float& v)
		{
			return _mm_cvtps_epi32(v.data);
		}

		static packed_float convert_float(const packed_int& v)
		{
			return _mm_cvtepi32_ps(v.data);
		}

		static packed_int cast_int(const packed_float &v)
		{
			return _mm_castps_si128(v.data);
		}

		static packed_float cast_float(const packed_int &v)
		{
			return _mm_castsi128_ps(v.data);
		}

		//////////////////////////////////////////////////////////////////////////
		// misc
		template<int hint>
		static void prefetch(const char* address)
		{
			_mm_prefetch(address, hint);
		}

		static void flushzero_on()
		{
			// Set DAZ and FZ MXCSR bits to flush denormals to zero (i.e., make it faster)
			// Denormal are zero (DAZ) is bit 6 and Flush to zero (FZ) is bit 15.
			// so to enable the two to have to set bits 6 and 15 which 1000 0000 0100 0000 = 0x8040
			_mm_setcsr(_mm_getcsr() | 0x8040);
		}

		static void flushzero_off()
		{
			_mm_setcsr(_mm_getcsr() & (~0x8040));
		}
	};


	//////////////////////////////////////////////////////////////////////////
	// SSE 3
	template <>
	struct simd_extension<sse_3> : public simd_extension<sse_2>
	{
		using simd_extension<sse_2>::abs;
		static packed_int abs(const packed_int& v)
		{
			return _mm_abs_epi32(v.data);
		}
	};


	//////////////////////////////////////////////////////////////////////////
	// SSE 4.1
	template <>
	struct simd_extension<sse_4_1> : public simd_extension<sse_3>
	{
		using simd_extension<sse_3>::min;
		static packed_int min(const packed_int& a, const packed_int& b)
		{
			return _mm_min_epi32(a.data, b.data);
		}

		using simd_extension<sse_3>::max;
		static packed_int max(const packed_int& a, const packed_int& b)
		{
			return _mm_max_epi32(a.data, b.data);
		}

		using simd_extension<sse_3>::min_lane;
		static int min_lane(const packed_int& v)
		{
			__m128i t = v.data;
			t = _mm_min_epi32(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(1, 0, 3, 2)));
			t = _mm_min_epi32(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(2, 3, 0, 1)));
			return _mm_cvtsi128_si32(t);
		}

		using simd_extension<sse_3>::max_lane;
		static int max_lane(const packed_int& v)
		{
			__m128i t = v.data;
			t = _mm_max_epi32(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(1, 0, 3, 2)));
			t = _mm_max_epi32(t, _mm_shuffle_epi32(t, _MM_SHUFFLE(2, 3, 0, 1)));
			return _mm_cvtsi128_si32(t);
		}

		static packed_float blend(const packed_float& a, const packed_float& b, const packed_float& mask)
		{
			return _mm_blendv_ps(a.data, b.data, mask.data);
		}

		static packed_int blend(const packed_int& a, const packed_int& b, const packed_int& mask)
		{
			__m128 t =_mm_blendv_ps(_mm_castsi128_ps(a.data), _mm_castsi128_ps(b.data), _mm_castsi128_ps(mask.data));
			return _mm_castps_si128(t);
		}

		using simd_extension<sse_3>::mul;
		static packed_int mul(const packed_int &a, const packed_int& b)
		{
			return _mm_mullo_epi32(a.data, b.data);
		}

		using simd_extension<sse_3>::muladd;
		static packed_int muladd(const packed_int& a, const packed_int& b, const packed_int& c)
		{
			return _mm_add_epi32(_mm_mullo_epi32(a.data, b.data), c.data);
		}

		static int allzeros(const packed_int& v) SIMD_COMPOSITE
		(
			return _mm_testz_si128(v.data, set_ones().data);
		)

		static int allzeros(const packed_int& v, const packed_int& mask)
		{
			return _mm_testz_si128(v.data, mask.data);
		}
	};
}

