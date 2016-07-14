#include "common/perf_case.hpp"
#include <thread>
#include <iostream>
#include <cstdint>

const int N = 10000000;
const int M = 10;

static uint8_t data[128];
static uint32_t count;

static void test0()
{
	for (int i = 0; i < N; ++i)
	{
		uint8_t s = data[i & 0xFF];
		uint32_t y = (s << 8) + 3;
		count += y;
	}
}

static void test1()
{
	for (int i = 0; i < N; ++i)
	{
		uint32_t y = (i << 8) + 3;
		count += y;
	}
}

REGISTER_PERF_SUITE(particle_register, false);

REGISTER_PERF_CASE(particle_register, test0, test0, M);
REGISTER_PERF_CASE(particle_register, test1, test1, M);

