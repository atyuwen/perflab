#pragma once

//  SSE 3 instructions emulation
__forceinline __m128i _mm_abs_epi32_emu(__m128i a)
{
	__m128i mask = _mm_cmplt_epi32( a, _mm_setzero_si128() ); // FFFF   where a < 0
	a    = _mm_xor_si128 ( a, mask );                         // Invert where a < 0
	mask = _mm_srli_epi32( mask, 31 );                        // 0001   where a < 0
	a    = _mm_add_epi32( a, mask );                          // Add 1  where a < 0
	return a;
}

// SSE 4.1 instructions emulation

__forceinline __m128i _mm_mullo_epi32_emu(__m128i a, __m128i b)
{
	__m128i tmp1 = _mm_mul_epu32(a, b); // mul 2, 0
	__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)); // mul 3, 1

	return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)),
		                      _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); // shuffle results to [63..0] and pack
}

// bitwise blend, not exactly the same as _mm_blendv_ps
__forceinline __m128 _mm_blendv_ps_emu( __m128 value, __m128 input, __m128 mask )
{ 
	return _mm_or_ps(_mm_and_ps(mask, input), _mm_andnot_ps(mask, value)); 
}

// bitwise blend, not exactly the same as _mm_blendv_epi8
__forceinline __m128i _mm_blendv_epi8_emu( __m128i value, __m128i input, __m128i mask )
{ 
	return _mm_or_si128(_mm_and_si128(mask, input), _mm_andnot_si128(mask, value)); 
}

__forceinline __m128i _mm_min_epi32_emu( __m128i value, __m128i input )
{ 
	return _mm_blendv_epi8_emu(input, value, _mm_cmplt_epi32(value, input));
}

__forceinline __m128i _mm_max_epi32_emu( __m128i value, __m128i input )
{ 
	return _mm_blendv_epi8_emu(value, input, _mm_cmplt_epi32(value, input));
}

__forceinline int _mm_testz_si128_emu(__m128i a, __m128i b)
{
	__m128i tmp = _mm_and_si128(a, b);
	tmp = _mm_packs_epi32(tmp, tmp);
	return tmp.m128i_u64[0] == 0;
}

