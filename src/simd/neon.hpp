#pragma once

#include "simd.hpp"
#include <arm_neon.h>

namespace simd
{
	template <>
	struct simd_extension<neon>
	{
		enum {arch = arm};
		enum {pack_size = 4};

		union packed_float
		{
			float32x4_t data;
			float lanes[4];

			packed_float() {}
			packed_float(const float32x4_t& v) : data(v) {}
			packed_float(const packed_float& v) : data(v.data) {}

			packed_float& operator = (const packed_float& v)
			{
				data = v.data;
				return *this;
			}

			float& operator [] (int lane)
			{
				return lanes[lane];
			}
		};

		union packed_int
		{
			int32x4_t data;
			int lanes[4];

			packed_int() {}
			packed_int(const int32x4_t& v) : data(v) {}
			packed_int(const packed_int& v) : data(v.data) {}

			packed_int& operator = (const packed_int& v)
			{
				data = v.data;
				return *this;
			}

			int& operator [] (int lane)
			{
				return lanes[lane];
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// packed float operations
		static packed_float load(const float* aligned_data)
		{
#if defined(_MSC_VER)
			return vld1q_f32_ex(aligned_data, 128);
#elif defined(__GNUC__)
			packed_float result;
			__asm__ (
				"vld1.32 {%q[result]}, [%[addr]:128]"
				: [result] "=w" (result)
				: [addr] "r" (aligned_data)
			);
			return result;
#else
			return vld1q_f32(aligned_data);
#endif
		}

		static packed_float load_unaligned(const float* unaligned_data)
		{
			return vld1q_f32(unaligned_data);
		}

		static void store(float* aligned_address, const packed_float& v)
		{
			return vst1q_f32(aligned_address, v.data);
		}

		static void stream(float* aligned_address, const packed_float& v)
		{
			// neon doesn't support streaming
			store(aligned_address, v);
		}

		static packed_float set(float v0, float v1, float v2, float v3)
		{
			float32x2_t A = vcreate_f32(((uint64_t)*(const uint32_t *)&v0) | ((uint64_t)(*(const uint32_t *)&v1) << 32));
			float32x2_t B = vcreate_f32(((uint64_t)*(const uint32_t *)&v2) | ((uint64_t)(*(const uint32_t *)&v3) << 32));
			return vcombine_f32(A, B);
		}

		static packed_float splat(float v0)
		{
			return vdupq_n_f32(v0);
		}

		static packed_float splat(float* pv0)
		{
			return vdupq_n_f32(*pv0);
		}

		template<int lane>
		static packed_float splat_lane(const packed_float& v);

		static packed_float set_zero()
		{
			return vdupq_n_f32(0.0f);
		}

		static packed_float set_one()
		{
			return vdupq_n_f32(1.0f);
		}

		static packed_float rcp_approx(const packed_float& v)
		{
			return vrecpeq_f32(v.data);
		}

		static packed_float rcp(const packed_float& v)
		{
			// 2 iterations of Newton-Raphson refinement of reciprocal
			float32x4_t r = vrecpeq_f32(v.data);
			r = vmulq_f32(vrecpsq_f32(v.data, r), r);
			r = vmulq_f32(vrecpsq_f32(v.data, r), r);
			return r;
		}

		static packed_float abs(const packed_float& v)
		{
			return vabsq_f32(v.data);
		}

		static packed_float min(const packed_float& a, const packed_float& b)
		{
			return vminq_f32(a.data, b.data);
		}

		static packed_float max(const packed_float& a, const packed_float& b)
		{
			return vmaxq_f32(a.data, b.data);
		}

		static float min_lane(const packed_float& v)
		{
			float32x2_t t = vpmin_f32(vget_low_f32(v.data), vget_high_f32(v.data));
			t = vpmin_f32(t, t);
			return vget_lane_f32(t, 0);
		}

		static float max_lane(const packed_float& v)
		{
			float32x2_t t = vpmax_f32(vget_low_f32(v.data), vget_high_f32(v.data));
			t = vpmax_f32(t, t);
			return vget_lane_f32(t, 0);
		}

		// saturate each lane to 16 bits and then pack to a 64-bit integer
		static long long digest(const packed_float& v)
		{
			int16x4_t t1 = vqmovn_s32(vreinterpretq_s32_f32(v.data));
			int64x1_t t2 = vreinterpret_s64_s16(t1);
			return vget_lane_s64(t2, 0);
		}

		// extract sign bits
		// not exactly the same as <sse>::movemask, only works if each lane is all zeros/ones
		static int movemask(const packed_float& v) SIMD_COMPOSITE
		(
			uint32x4_t mask = vreinterpretq_u32_s32(set(1, 2, 4, 8).data);
			uint32x4_t t0 = vandq_u32(mask, vreinterpretq_u32_f32(v.data));
			uint32x2_t t1 = vorr_u32(vget_low_u32(t0), vget_high_u32(t0));
			uint32x2_t t2 = vpadd_u32(t1, t1);
			return (int)vget_lane_u32(t2, 0);
		)

		// return mask.lanes[i].sign_bit ? b.lanes[i] : a.lanes[i]
		static packed_float blend(const packed_float& a, const packed_float& b, const packed_float& mask)
		{
			uint32x4_t t = vcltq_s32(vreinterpretq_s32_f32(mask.data), vdupq_n_s32(0));
			return vbslq_f32(t, b.data, a.data);
		}

		// return mask.bits[i] ? b.bits[i] : a.bits[i]
		static packed_float bitwise_blend(const packed_float& a, const packed_float& b, const packed_float& mask)
		{
			return vbslq_f32(vreinterpretq_u32_f32(mask.data), b.data, a.data);
		}

		static packed_float add(const packed_float& a, const packed_float& b)
		{
			return vaddq_f32(a.data, b.data);
		}

		static packed_float sub(const packed_float &a, const packed_float& b)
		{
			return vsubq_f32(a.data, b.data);
		}

		static packed_float mul(const packed_float &a, const packed_float& b)
		{
			return vmulq_f32(a.data, b.data);
		}

		// return a * b + c
		static packed_float muladd(const packed_float& a, const packed_float& b, const packed_float& c)
		{
			return vmlaq_f32(c.data, a.data, b.data);
		}

		static packed_float div(const packed_float &a, const packed_float& b)
		{
			return vmulq_f32(a.data, rcp(b).data);
		}

		static packed_float bitwise_and(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(a.data), vreinterpretq_u32_f32(b.data)));
		}

		static packed_float bitwise_or(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vorrq_u32(vreinterpretq_u32_f32(a.data), vreinterpretq_u32_f32(b.data)));
		}

		static packed_float bitwise_xor(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(a.data), vreinterpretq_u32_f32(b.data)));
		}

		static packed_float bitwise_not(const packed_float &a)
		{
			return vreinterpretq_f32_u32(vmvnq_u32(vreinterpretq_u32_f32(a.data)));
		}

		// return (~a) & b
		static packed_float bitwise_andnot(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vandq_u32(vmvnq_u32(vreinterpretq_u32_f32(a.data)), vreinterpretq_u32_f32(b.data)));
		}

		static packed_float is_greater(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vcgtq_f32(a.data, b.data));
		}

		static packed_float is_greater_equal(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vcgeq_f32(a.data, b.data));
		}

		static packed_float is_less(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vcltq_f32(a.data, b.data));
		}

		static packed_float is_less_equal(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vcleq_f32(a.data, b.data));
		}

		static packed_float is_equal(const packed_float &a, const packed_float& b)
		{
			return vreinterpretq_f32_u32(vceqq_f32(a.data, b.data));
		}

		static packed_float merge_low(const packed_float& a, const packed_float &b)
		{
			return vcombine_f32(vget_low_f32(a.data), vget_low_f32(b.data));
		}

		static packed_float merge_high(const packed_float& a, const packed_float &b)
		{
			return vcombine_f32(vget_high_f32(a.data), vget_high_f32(b.data));
		}

		static packed_float merge_odd(const packed_float& a, const packed_float &b)
		{
			return vuzpq_f32(a.data, b.data).val[0];
		}

		static packed_float merge_even(const packed_float& a, const packed_float &b)
		{
			return vuzpq_f32(a.data, b.data).val[1];
		}

		static void transpose(packed_float& r0, packed_float& r1, packed_float& r2, packed_float &r3)
		{
			float32x4x2_t P0 = vzipq_f32(r0.data, r2.data);
			float32x4x2_t P1 = vzipq_f32(r1.data, r3.data);
			float32x4x2_t T0 = vzipq_f32(P0.val[0], P1.val[0]);
			float32x4x2_t T1 = vzipq_f32(P0.val[1], P1.val[1]);
			r0 = T0.val[0];
			r1 = T0.val[1];
			r2 = T1.val[0];
			r3 = T1.val[1];
		}

		//////////////////////////////////////////////////////////////////////////
		// packed int operations
		static packed_int load(const int* aligned_data)
		{
#if defined(_MSC_VER)
			return vld1q_s32_ex(aligned_data, 128);
#elif defined(__GNUC__)
			packed_int result;
			__asm__ (
				"vld1.32 {%q[result]}, [%[addr]:128]"
				: [result] "=w" (result)
				: [addr] "r" (aligned_data)
				);
			return result;
#else
			return vld1q_s32(aligned_data);
#endif
		}

		static packed_int load_unaligned(const int* unaligned_data)
		{
			return vld1q_s32(unaligned_data);
		}

		static void store(int* aligned_address, const packed_int& v)
		{
			return vst1q_s32(aligned_address, v.data);
		}

		static void stream(int* aligned_address, const packed_int& v)
		{
			// neon doesn't support streaming
			store(aligned_address, v);
		}

		static packed_int set(int v0, int v1, int v2, int v3)
		{
			int32x2_t A = vcreate_s32(((uint64_t)*(const uint32_t *)&v0) | ((uint64_t)(*(const uint32_t *)&v1) << 32));
			int32x2_t B = vcreate_s32(((uint64_t)*(const uint32_t *)&v2) | ((uint64_t)(*(const uint32_t *)&v3) << 32));
			return vcombine_s32(A, B);
		}

		static packed_int splat(int v0)
		{
			return vdupq_n_s32(v0);
		}

		template<int lane>
		static packed_int splat_lane(const packed_int& v);

		static packed_int set_zeros()
		{
			return vdupq_n_s32(0);
		}

		static packed_int set_ones()
		{
			return vdupq_n_s32(0xFFFFFFFF);
		}

		static packed_int abs(const packed_int& v)
		{
			return vabsq_s32(v.data);
		}

		static packed_int min(const packed_int& a, const packed_int& b)
		{
			return vminq_s32(a.data, b.data);
		}

		static packed_int max(const packed_int& a, const packed_int& b)
		{
			return vmaxq_s32(a.data, b.data);
		}

		static int min_lane(const packed_int& v)
		{
			int32x2_t t = vpmin_s32(vget_low_s32(v.data), vget_high_s32(v.data));
			t = vpmin_s32(t, t);
			return vget_lane_s32(t, 0);
		}

		static int max_lane(const packed_int& v)
		{
			int32x2_t t = vpmax_s32(vget_low_s32(v.data), vget_high_s32(v.data));
			t = vpmax_s32(t, t);
			return vget_lane_s32(t, 0);
		}

		// saturate each lane to 16 bits and then pack to a 64-bit integer
		static long long digest(const packed_int& v)
		{
			int16x4_t t1 = vqmovn_s32(v.data);
			int64x1_t t2 = vreinterpret_s64_s16(t1);
			return vget_lane_s64(t2, 0);
		}

		// extract sign bits
		// not exactly the same as <sse>::movemask, only works if each lane is all zeros/ones
		static int movemask(const packed_int& v) SIMD_COMPOSITE
		(
			uint32x4_t mask = vreinterpretq_u32_s32(set(1, 2, 4, 8).data);
			uint32x4_t t0 = vandq_u32(mask, vreinterpretq_u32_s32(v.data));
			uint32x2_t t1 = vorr_u32(vget_low_u32(t0), vget_high_u32(t0));
			uint32x2_t t2 = vpadd_u32(t1, t1);
			return (int)vget_lane_u32(t2, 0);
		)

		// return mask.lanes[i].sign_bit ? b.lanes[i] : a.lanes[i]
		static packed_int blend(const packed_int& a, const packed_int& b, const packed_int& mask)
		{
			uint32x4_t t = vcltq_s32(mask.data, vdupq_n_s32(0));
			return vbslq_s32(t, b.data, a.data);
		}

		// return mask.bits[i] ? b.bits[i] : a.bits[i]
		static packed_int bitwise_blend(const packed_int& a, const packed_int& b, const packed_int& mask)
		{
			return vbslq_s32(vreinterpretq_u32_s32(mask.data), b.data, a.data);
		}

		static packed_int add(const packed_int& a, const packed_int& b)
		{
			return vaddq_s32(a.data, b.data);
		}

		static packed_int sub(const packed_int &a, const packed_int& b)
		{
			return vsubq_s32(a.data, b.data);
		}

		static packed_int mul(const packed_int &a, const packed_int& b)
		{
			return vmulq_s32(a.data, b.data);
		}

		// return a * b + c
		static packed_int muladd(const packed_int& a, const packed_int& b, const packed_int& c)
		{
			return vmlaq_s32(c.data, a.data, b.data);
		}

		static packed_int bitwise_and(const packed_int &a, const packed_int& b)
		{
			return vandq_s32(a.data, b.data);
		}

		static packed_int bitwise_or(const packed_int &a, const packed_int& b)
		{
			return vorrq_s32(a.data, b.data);
		}

		static packed_int bitwise_xor(const packed_int &a, const packed_int& b)
		{
			return veorq_s32(a.data, b.data);
		}

		static packed_int bitwise_not(const packed_int &a)
		{
			return vmvnq_s32(a.data);
		}

		// return (~a) & b
		static packed_int bitwise_andnot(const packed_int &a, const packed_int& b)
		{
			return vandq_s32(vmvnq_s32(a.data), b.data);
		}

		static packed_int is_greater(const packed_int &a, const packed_int& b)
		{
			return vreinterpretq_s32_u32(vcgtq_s32(a.data, b.data));
		}

		static packed_int is_greater_equal(const packed_int &a, const packed_int& b)
		{
			return vreinterpretq_s32_u32(vcgeq_s32(a.data, b.data));
		}

		static packed_int is_less(const packed_int &a, const packed_int& b)
		{
			return vreinterpretq_s32_u32(vcltq_s32(a.data, b.data));
		}

		static packed_int is_less_equal(const packed_int &a, const packed_int& b)
		{
			return vreinterpretq_s32_u32(vcleq_s32(a.data, b.data));
		}

		static packed_int is_equal(const packed_int &a, const packed_int& b)
		{
			return vreinterpretq_s32_u32(vceqq_s32(a.data, b.data));
		}

		template<int n>
		static packed_int shift_right(const packed_int &a)
		{
			return vshrq_n_s32(a.data, n);
		}

		template<int n>
		static packed_int shift_left(const packed_int &a)
		{
			return vshlq_n_s32(a.data, n);
		}

		static int allzeros(const packed_int& v)
		{
			int16x4_t t1 = vqmovn_s32(v.data);
			int64x1_t t2 = vreinterpret_s64_s16(t1);
			return vget_lane_s64(t2, 0) == 0;
		}

		static int allzeros(const packed_int& v, const packed_int& mask)
		{
			return allzeros(vandq_s32(v.data, mask.data));
		}

		//////////////////////////////////////////////////////////////////////////
		// packed int / float converting/casting
		static packed_int convert_int(const packed_float& v)
		{
			return vcvtq_s32_f32(v.data);
		}

		static packed_float convert_float(const packed_int& v)
		{
			return vcvtq_f32_s32(v.data);
		}

		static packed_int cast_int(const packed_float &v)
		{
			return vreinterpretq_s32_f32(v.data);
		}

		static packed_float cast_float(const packed_int &v)
		{
			return vreinterpretq_f32_s32(v.data);
		}

		//////////////////////////////////////////////////////////////////////////
		// misc
		template<int hint>
		static void prefetch(const char* address)
		{
#if defined(__GNUC__)
			__builtin_prefetch(address);
#endif
		}

		static void flushzero_on()
		{
			// neon always flush denormals to zero
		}

		static void flushzero_off()
		{

		}
	};


	//////////////////////////////////////////////////////////////////////////
	// template specializations
	template<> inline
	simd_extension<neon>::packed_float simd_extension<neon>::splat_lane<0>(const packed_float& v)
	{
		return vdupq_lane_f32(vget_low_f32(v.data), 0);
	}
	template<> inline
	simd_extension<neon>::packed_float simd_extension<neon>::splat_lane<1>(const packed_float& v)
	{
		return vdupq_lane_f32(vget_low_f32(v.data), 1);
	}
	template<> inline
	simd_extension<neon>::packed_float simd_extension<neon>::splat_lane<2>(const packed_float& v)
	{
		return vdupq_lane_f32(vget_high_f32(v.data), 0);
	}
	template<> inline
	simd_extension<neon>::packed_float simd_extension<neon>::splat_lane<3>(const packed_float& v)
	{
		return vdupq_lane_f32(vget_high_f32(v.data), 1);
	}

	template<> inline
	simd_extension<neon>::packed_int simd_extension<neon>::splat_lane<0>(const packed_int& v)
	{
		return vdupq_lane_s32(vget_low_s32(v.data), 0);
	}
	template<> inline
	simd_extension<neon>::packed_int simd_extension<neon>::splat_lane<1>(const packed_int& v)
	{
		return vdupq_lane_s32(vget_low_s32(v.data), 1);
	}
	template<> inline
	simd_extension<neon>::packed_int simd_extension<neon>::splat_lane<2>(const packed_int& v)
	{
		return vdupq_lane_s32(vget_high_s32(v.data), 0);
	}
	template<> inline
	simd_extension<neon>::packed_int simd_extension<neon>::splat_lane<3>(const packed_int& v)
	{
		return vdupq_lane_s32(vget_high_s32(v.data), 1);
	}

}

