#include "common/perf_case.hpp"
#include "simd/simd.hpp"

#if defined(_M_IX86)
typedef simd::simd_extension<simd::sse_4_1> simd_ext;
#else
typedef simd::simd_extension<simd::neon> simd_ext;
#endif

const int N = 1000000;
const int M = 10;

struct tester : public simd_ext
{
	static void add_scalar()
	{

	}

	static void add_simd()
	{

	}
};

REGISTER_PERF_SUITE(simd, false);

REGISTER_PERF_CASE(simd, add_scalar, tester::add_scalar, M);
REGISTER_PERF_CASE(simd, add_simd,   tester::add_simd,   M);

