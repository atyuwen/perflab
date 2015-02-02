
#include <cmath>
#include <random>

#include "common/perf_case.hpp"
#include "common/fast_math.hpp"

const int N = 1000000;
const int M = 10;

static float *data = nullptr;

static void intialize()
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0.1f, 2.0f);

	data = new float[N];
	for (int i = 0; i < N; ++i)
	{
		data[i] = distribution(generator);
	}
}

static void finish()
{
	delete [] data;
}

#define DEFINE_PERF_CASE_ONE_PARAM(func) \
static void test_##func() \
{ \
	float ret = 0; \
	for (int i = 0; i < N; ++i) \
	{ \
		ret += func(data[i]); \
	} \
	PERF_MSG(msg_info, "ret = %f", ret); \
} \
REGISTER_PERF_CASE(math, func, test_##func, M);

#define DEFINE_PERF_CASE_TWO_PARAMS(func) \
static void test_##func() \
{ \
	float ret = 0; \
	for (int i = 0; i < N; ++i) \
	{ \
		ret += func(2.0f, data[i]); \
	} \
	PERF_MSG(msg_info, "ret = %f", ret); \
} \
REGISTER_PERF_CASE(math, func, test_##func, M);

REGISTER_PERF_SUITE(math, false, intialize, finish);

DEFINE_PERF_CASE_ONE_PARAM(abs);
DEFINE_PERF_CASE_ONE_PARAM(fast_abs);

DEFINE_PERF_CASE_ONE_PARAM(sqrt);
DEFINE_PERF_CASE_ONE_PARAM(fast_sqrt);
DEFINE_PERF_CASE_ONE_PARAM(exp);
DEFINE_PERF_CASE_ONE_PARAM(log);

DEFINE_PERF_CASE_ONE_PARAM(sin);
DEFINE_PERF_CASE_ONE_PARAM(fast_sin);
DEFINE_PERF_CASE_ONE_PARAM(cos);
DEFINE_PERF_CASE_ONE_PARAM(fast_cos);
DEFINE_PERF_CASE_ONE_PARAM(tan);
DEFINE_PERF_CASE_ONE_PARAM(asin);
DEFINE_PERF_CASE_ONE_PARAM(acos);
DEFINE_PERF_CASE_ONE_PARAM(atan);

DEFINE_PERF_CASE_TWO_PARAMS(pow);
DEFINE_PERF_CASE_TWO_PARAMS(atan2);

