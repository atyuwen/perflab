#pragma once

#include <cstdint>

// this file collect some fast but "inaccurate" or "not C++ std compliant" mathematic routines.
// for those scenarios where performance is extremely critical so that accuracy can be sacrificed.

typedef union
{
	int32_t i;
	float   f;
} _INTORFLOAT;

#define FTOI_MAGIC_NUM   12582912.0f  // 1.5 * 2^23
#define MATH_2PI         6.28318531f  // 2 pi

extern const float _sin_table[1024];

// the absolute errors are between about -0.006 and 0.006
inline float fast_sin(float theta)
{
	_INTORFLOAT ftmp;
	ftmp.f = theta * (1024.0f / MATH_2PI) + FTOI_MAGIC_NUM;
	int32_t i = ftmp.i & 1023;
	return _sin_table[i];
}

inline float fast_cos(float theta)
{
	_INTORFLOAT ftmp;
	ftmp.f = theta * (1024.0f / MATH_2PI) + FTOI_MAGIC_NUM + 256.0f;
	int32_t i = ftmp.i & 1023;
	return _sin_table[i];
}

inline float fast_abs(float v)
{
	_INTORFLOAT ftmp;
	ftmp.f = v;
	ftmp.i &= 0x7fffffff;
	return ftmp.f;
}

// from Quake III source
inline float fast_inv_sqrt(float v)
{
	_INTORFLOAT ftmp;
	ftmp.f = v;
	ftmp.i = 0x5f3759df - (ftmp.i >> 1);
	float x = v * 0.5f;
	ftmp.f *= 1.5f - x * ftmp.f * ftmp.f;  // this line can be repeated arbitrarily many times to increase accuracy
	return ftmp.f;
}

#if FAST_SQRT_COARSE
// the errors are between about -3.5% and 3.5%.
inline float fast_sqrt(float v)
{
	_INTORFLOAT ftmp;
	ftmp.f = v;
	ftmp.i = 0x1fbb4000 + (ftmp.i >> 1);
	return ftmp.f;
}
#else
// use "v * fast_inv_sqrt(v)" for better precision.
inline float fast_sqrt(float v)
{
	return v * fast_inv_sqrt(v);
}
#endif

// clamp the value to [0, 1] range
inline float fast_saturate(float v)
{
	_INTORFLOAT ftmp;
	ftmp.f = v;
	ftmp.i &= ~(ftmp.i >> 31);
	ftmp.f -= 1.0f;
	ftmp.i &= (ftmp.i >> 31);
	ftmp.f += 1.0f;
	return ftmp.f;
}

