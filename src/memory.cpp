#include <cstdint>
#include <cassert>
#include <vector>

#include "common/perf_case.hpp"
#include "common/cache.hpp"

const int N = 1 * 1024 * 1024;

static int32_t *data = nullptr;
static std::vector<char*> trashes;

static void clear_cache_clflush()
{
	flush_cache(data, data + N);
}

static void clear_cache_sandybridgei5()
{
	const int cache_size = 6 * 1024 * 1024;
	const int cache_line = 64;
	char* trash = (char*)alloc_large_page(cache_size);
	int ret = 0;
	for (int i = 0; i < cache_size; i += cache_line)
	{
		ret += trash[i];
	}
	PERF_MSG(msg_info, "clear_cache, ret = %d", ret);
	trashes.emplace_back(trash);
}

static void initialize()
{
	init_large_page();
	data = new int32_t[N];
}

static void finish()
{
	delete [] data;

	for (char* trash : trashes)
	{
		free_large_page(trash);
	}
}

static void mem_read()
{
	int ret = 0;
	for (int i = 0; i < N; ++i)
	{
		ret += data[i];
	}
	PERF_MSG(msg_info, "mem_read, ret = %d", ret);
}

static void mem_write()
{

}

static void mem_stream()
{

}

REGISTER_PERF_SUITE(memory, true, initialize, finish);

REGISTER_PERF_CASE(memory, read1, mem_read, 1);
REGISTER_PERF_CASE(memory, read2, mem_read, 1);
REGISTER_PERF_CASE(memory, clear1, clear_cache_sandybridgei5, 1);
REGISTER_PERF_CASE(memory, read3, mem_read, 1);


