#include "common/perf_case.hpp"
#include "common/common.hpp"
#include <thread>
#include <iostream>

const int N = 1000000;
const int M = 10;

ALIGN(16) static volatile char data[128 + 4];

static void aligned()
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < 128; j += 4)
		{
			float* t = (float*)(data + j);
			*t += i;
			*t += j;
		}
	}
}

static void unaligned()
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = 1; j < 128; j += 4)
		{
			float* t = (float*)(data + j);
			*t += i;
			*t += j;
		}
	}
}

REGISTER_PERF_SUITE(unaligned_access, true);

REGISTER_PERF_CASE(unaligned_access, aligned,   aligned,   M);
REGISTER_PERF_CASE(unaligned_access, unaligned, unaligned, M);

