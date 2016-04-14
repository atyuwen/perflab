#include "common/perf_case.hpp"
#include "simd/simd.hpp"

const int N = 1000000;
const int M = 10;

struct int4
{
	int x, y, z, w;
};

struct int4m
{
	int x, y, z, w;

	int4m& operator = (const int4m &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}
};

struct char4u
{
	union
	{
		struct
		{
			char x, y, z, w;
		};
		int d;
	};
};

int4 out[16];
int4m outm[16];
char4u outu[16];

static void implicit()
{
	int4 a = {0, 0, 0, 0};
	for (int i = 0;i < N; ++i)
	{
		a.x = i;
		int idx  = i & 0xF;
		out[idx] = a;
	}
}

static void custom()
{
	int4m a = {0, 0, 0, 0};
	for (int i = 0;i < N; ++i)
	{
		a.x = i;
		int idx  = i & 0xF;
		outm[idx] = a;
	}
}

static void piecewise()
{
	int4m a = {0, 0, 0, 0};
	for (int i = 0;i < N; ++i)
	{
		a.x = i;
		int idx  = i & 0xF;
		out[idx].x = a.x;
		out[idx].y = a.y;
		out[idx].z = a.z;
		out[idx].w = a.w;
	}
}

static void test_union()
{
	char4u a;
	a.d = 0;
	for (int i = 0;i < N; ++i)
	{
		a.x = i;
		int idx  = i & 0xF;
		outu[idx].x = a.x;
		outu[idx].y = a.y;
	}
}

static void test_union2()
{
	char4u a;
	a.d = 0;
	for (int i = 0;i < N; ++i)
	{
		a.x = i;
		int idx  = i & 0xF;
		outu[idx].d = a.d;
	}
}

REGISTER_PERF_SUITE(store_forwarding, false);

REGISTER_PERF_CASE(store_forwarding, implicit,    implicit,    M);
REGISTER_PERF_CASE(store_forwarding, custom,      custom,      M);
REGISTER_PERF_CASE(store_forwarding, piecewise,   piecewise,   M);
REGISTER_PERF_CASE(store_forwarding, test_union,  test_union,  M);
REGISTER_PERF_CASE(store_forwarding, test_union2, test_union2, M);
