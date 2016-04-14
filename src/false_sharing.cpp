#include "common/perf_case.hpp"
#include "common/common.hpp"
#include <thread>
#include <iostream>

const int N = 1000000;
const int M = 10;

ALIGN(64) static volatile int count[128];

template<int start, int end, int step>
static void func()
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = start; j < end; j += step)
		{
			count[j] += i;
			count[j] += j;
		}
	}
}

static void shared()
{
	std::thread a(func<0, 128, 2>);
	std::thread b(func<1, 128, 2>);
	a.join();
	b.join();
}

static void unshared()
{
	std::thread a(func<0,  64, 1>);
	std::thread b(func<64, 28, 1>);
	a.join();
	b.join();
}

REGISTER_PERF_SUITE(false_sharing, true);

REGISTER_PERF_CASE(false_sharing, shared,   shared,   M);
REGISTER_PERF_CASE(false_sharing, unshared, unshared, M);

