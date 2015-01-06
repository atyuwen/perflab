#include "common/perf_case.hpp"

const int N = 100000000;
const int M = 10;

void initialize()
{

}

void finish()
{

}

void add_int_s1()
{
	int ret = 0;
	for (int i = 0; i < N; ++i)
	{
		ret += i;
	}
	PERF_MSG(msg_info, "ret = %d", ret);
}

void add_int_s2()
{
	int ret0 = 0;
	int ret1 = 0;
	for (int i = 0; i < N; i += 2)
	{
		ret0 += i;
		ret1 += i + 1;
	}
	PERF_MSG(msg_info, "ret = %d", ret0 + ret1);
}

REGISTER_PERF_SUITE(arithmetic, true, initialize, finish);

REGISTER_PERF_CASE(arithmetic, add_int_s1, add_int_s1, M);
REGISTER_PERF_CASE(arithmetic, add_int_s2, add_int_s2, M);


