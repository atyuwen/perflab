#include "common/perf_case.hpp"

#include <cstdint>
#include <cstdlib>

const int N = 1000000;
const int M = 10;

#if 1
int my_rand()
{
	static int r = 0;
#ifdef _MSC_VER
	r = (r * 214013 + 2531011) & 0x7fff;
#else
	r = (r * 214013 + 2531011) & 0x7fffffff;
#endif
	return r;
}
#define rand my_rand
#endif

int random_int(int n)
{
	return rand() % n;
}

int random_int_fast(int n)
{
#ifdef _MSC_VER
	return rand() * (int64_t)(n) >> 15;
#else
	return rand() * (int64_t)(n) >> 31;
#endif
}

static void random_int_test()
{
	int sum = 0;
	int n = random_int(3) + 10;
	for (int i = 0; i < N; ++i)
	{
		sum += random_int(n);
	}
	PERF_MSG(msg_info, "n=%d, avg = %f", n, (float)sum / N);
}

static void random_int_fast_test()
{
	int sum = 0;
	int n = random_int_fast(3) + 10;
	for (int i = 0; i < N; ++i)
	{
		sum += random_int_fast(n);
	}
	PERF_MSG(msg_info, "n=%d, avg = %f", n, (float)sum / N);
}

REGISTER_PERF_SUITE(random, true);

REGISTER_PERF_CASE(random, rand_int,      random_int_test,      M);
REGISTER_PERF_CASE(random, rand_int_fast, random_int_fast_test, M);
