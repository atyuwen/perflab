#include "common/perf_case.hpp"

const int N = 10000000;
const int M = 10;

template <int num, int step> 
static void add_int()
{
	static_assert(step <= 6, "step <= 6");

	int t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
	for (int i = 0; i < num; i += step)
	{
		if (step > 0) t0 += i;
		if (step > 1) t1 += i;
		if (step > 2) t2 += i;
		if (step > 3) t3 += i;
		if (step > 4) t4 += i;
		if (step > 5) t5 += i;
	}

	int ret = t0 + t1 + t2 + t3 + t4 + t5;
	PERF_MSG(msg_info, "ret = %d", ret);
}

template <int num, int step> 
static void add_float()
{
	static_assert(step <= 6, "step <= 6");

	float t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
	for (int i = 0; i < num; i += step)
	{
		if (step > 0) t0 += 1.0f;
		if (step > 1) t1 += 1.0f;
		if (step > 2) t2 += 1.0f;
		if (step > 3) t3 += 1.0f;
		if (step > 4) t4 += 1.0f;
		if (step > 5) t5 += 1.0f;
	}

	float ret = t0 + t1 + t2 + t3 + t4 + t5;
	PERF_MSG(msg_info, "ret = %f", ret);
}

template <int num, int step> 
static void mul_int()
{
	static_assert(step <= 6, "step <= 6");

	int t0 = 1, t1 = 1, t2 = 1, t3 = 1, t4 = 1, t5 = 1;
	for (int i = 0; i < num; i += step)
	{
		if (step > 0) t0 *= i;
		if (step > 1) t1 *= i;
		if (step > 2) t2 *= i;
		if (step > 3) t3 *= i;
		if (step > 4) t4 *= i;
		if (step > 5) t5 *= i;
	}

	int ret = t0 * t1 * t2 * t3 * t4 * t5;
	PERF_MSG(msg_info, "ret = %d", ret);
}

template <int num, int step> 
static void mul_float()
{
	static_assert(step <= 6, "step <= 6");

	float t0 = 1, t1 = 1, t2 = 1, t3 = 1, t4 = 1, t5 = 1;
	for (int i = 0; i < num; i += step)
	{
		if (step > 0) t0 *= 1.000001f;
		if (step > 1) t1 *= 1.000001f;
		if (step > 2) t2 *= 1.000001f;
		if (step > 3) t3 *= 1.000001f;
		if (step > 4) t4 *= 1.000001f;
		if (step > 5) t5 *= 1.000001f;
	}

	float ret = t0 * t1 * t2 * t3 * t4 * t5;
	PERF_MSG(msg_info, "ret = %f", ret);
}

template <int num, int step> 
static void div_int()
{
	static_assert(step <= 6, "step <= 6");

	const int c = 2^30;
	int t0 = c, t1 = c, t2 = c, t3 = c, t4 = c, t5 = c;
	for (int i = 1; i < num; i += step)
	{
		if (step > 0) t0 /= i;
		if (step > 1) t1 /= i;
		if (step > 2) t2 /= i;
		if (step > 3) t3 /= i;
		if (step > 4) t4 /= i;
		if (step > 5) t5 /= i;
	}

	int ret = t0 * t1 * t2 * t3 * t4 * t5;
	PERF_MSG(msg_info, "ret = %d", ret);
}

template <int num, int step> 
static void div_float()
{
	static_assert(step <= 6, "step <= 6");

	const float c = 1e9;
	float t0 = c, t1 = c, t2 = c, t3 = c, t4 = c, t5 = c;
	for (int i = 0; i < num; i += step)
	{
		if (step > 0) t0 /= 1.000001f;
		if (step > 1) t1 /= 1.000001f;
		if (step > 2) t2 /= 1.000001f;
		if (step > 3) t3 /= 1.000001f;
		if (step > 4) t4 /= 1.000001f;
		if (step > 5) t5 /= 1.000001f;
	}

	float ret = t0 * t1 * t2 * t3 * t4 * t5;
	PERF_MSG(msg_info, "ret = %f", ret);
}

template <int num, int step> 
static void mod_int()
{
	static_assert(step <= 6, "step <= 6");

	const int c = 2^30;
	int t0 = c, t1 = c, t2 = c, t3 = c, t4 = c, t5 = c;
	for (int i = num; i > 0; i -= step)
	{
		if (step > 0) t0 %= i;
		if (step > 1) t1 %= i;
		if (step > 2) t2 %= i;
		if (step > 3) t3 %= i;
		if (step > 4) t4 %= i;
		if (step > 5) t5 %= i;
	}

	int ret = t0 * t1 * t2 * t3 * t4 * t5;
	PERF_MSG(msg_info, "ret = %d", ret);
}

REGISTER_PERF_SUITE(arithmetic, false);

REGISTER_PERF_CASE(arithmetic, add_int_s1, (add_int<N, 1>), M);
REGISTER_PERF_CASE(arithmetic, add_int_s2, (add_int<N, 2>), M);
REGISTER_PERF_CASE(arithmetic, add_int_s3, (add_int<N, 3>), M);
REGISTER_PERF_CASE(arithmetic, add_int_s4, (add_int<N, 4>), M);
REGISTER_PERF_CASE(arithmetic, add_int_s5, (add_int<N, 5>), M);
REGISTER_PERF_CASE(arithmetic, add_int_s6, (add_int<N, 6>), M);

REGISTER_PERF_CASE(arithmetic, add_float_s1, (add_float<N, 1>), M);
REGISTER_PERF_CASE(arithmetic, add_float_s2, (add_float<N, 2>), M);
REGISTER_PERF_CASE(arithmetic, add_float_s3, (add_float<N, 3>), M);
REGISTER_PERF_CASE(arithmetic, add_float_s4, (add_float<N, 4>), M);
REGISTER_PERF_CASE(arithmetic, add_float_s5, (add_float<N, 5>), M);
REGISTER_PERF_CASE(arithmetic, add_float_s6, (add_float<N, 6>), M);

REGISTER_PERF_CASE(arithmetic, mul_int_s1, (mul_int<N, 1>), M);
REGISTER_PERF_CASE(arithmetic, mul_int_s2, (mul_int<N, 2>), M);
REGISTER_PERF_CASE(arithmetic, mul_int_s3, (mul_int<N, 3>), M);
REGISTER_PERF_CASE(arithmetic, mul_int_s4, (mul_int<N, 4>), M);
REGISTER_PERF_CASE(arithmetic, mul_int_s5, (mul_int<N, 5>), M);
REGISTER_PERF_CASE(arithmetic, mul_int_s6, (mul_int<N, 6>), M);

REGISTER_PERF_CASE(arithmetic, mul_float_s1, (mul_float<N, 1>), M);
REGISTER_PERF_CASE(arithmetic, mul_float_s2, (mul_float<N, 2>), M);
REGISTER_PERF_CASE(arithmetic, mul_float_s3, (mul_float<N, 3>), M);
REGISTER_PERF_CASE(arithmetic, mul_float_s4, (mul_float<N, 4>), M);
REGISTER_PERF_CASE(arithmetic, mul_float_s5, (mul_float<N, 5>), M);
REGISTER_PERF_CASE(arithmetic, mul_float_s6, (mul_float<N, 6>), M);

REGISTER_PERF_CASE(arithmetic, div_int_s1, (div_int<N, 1>), M);
REGISTER_PERF_CASE(arithmetic, div_int_s2, (div_int<N, 2>), M);
REGISTER_PERF_CASE(arithmetic, div_int_s3, (div_int<N, 3>), M);
REGISTER_PERF_CASE(arithmetic, div_int_s4, (div_int<N, 4>), M);
REGISTER_PERF_CASE(arithmetic, div_int_s5, (div_int<N, 5>), M);
REGISTER_PERF_CASE(arithmetic, div_int_s6, (div_int<N, 6>), M);

REGISTER_PERF_CASE(arithmetic, div_float_s1, (div_float<N, 1>), M);
REGISTER_PERF_CASE(arithmetic, div_float_s2, (div_float<N, 2>), M);
REGISTER_PERF_CASE(arithmetic, div_float_s3, (div_float<N, 3>), M);
REGISTER_PERF_CASE(arithmetic, div_float_s4, (div_float<N, 4>), M);
REGISTER_PERF_CASE(arithmetic, div_float_s5, (div_float<N, 5>), M);
REGISTER_PERF_CASE(arithmetic, div_float_s6, (div_float<N, 6>), M);

REGISTER_PERF_CASE(arithmetic, mod_int_s1, (mod_int<N, 1>), M);
REGISTER_PERF_CASE(arithmetic, mod_int_s2, (mod_int<N, 2>), M);
REGISTER_PERF_CASE(arithmetic, mod_int_s3, (mod_int<N, 3>), M);
REGISTER_PERF_CASE(arithmetic, mod_int_s4, (mod_int<N, 4>), M);
REGISTER_PERF_CASE(arithmetic, mod_int_s5, (mod_int<N, 5>), M);
REGISTER_PERF_CASE(arithmetic, mod_int_s6, (mod_int<N, 6>), M);

