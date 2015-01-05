#include <atomic>
#include <thread>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>

#include "common/perf_case.hpp"

const int N = 100000;
const int M = 10;

template<bool atomic, std::memory_order order1, std::memory_order order2>
class my_queue
{
public:
	my_queue()
		: read_pos_(0)
		, write_pos_(0)
		, atomic_write_pos_(0)
	{
		data_ = new int32_t[N];
		memset(data_, 0, sizeof(int32_t) * N);
	}

	~my_queue()
	{
		delete [] data_;
	}

public:
	void push_back(int32_t val)
	{
		if (!atomic)
		{
			data_[write_pos_] = val;
			++write_pos_;
		}
		else
		{
			int32_t write_pos = atomic_write_pos_.load(std::memory_order_relaxed);
			data_[write_pos] = val;
			atomic_write_pos_.fetch_add(1, order2);
		}
	}

	bool pop_front(int32_t& retval)
	{
		if (!atomic)
		{
			if (read_pos_ < write_pos_)
			{
				retval = data_[read_pos_];
				++read_pos_;
				return true;
			}
			return false;
		}
		else
		{
			if (read_pos_ < atomic_write_pos_.load(order1))
			{
				retval = data_[read_pos_];
				++read_pos_;
				return true;
			}
			return false;
		}
	}

private:
	int32_t               read_pos_;
	volatile int32_t      write_pos_;
	std::atomic<int32_t>  atomic_write_pos_;
	int32_t*              data_;
};

template<bool atomic, std::memory_order order1, std::memory_order order2>
static void test()
{
	my_queue<atomic, order1, order2> q;

	int32_t pushed = 0;
	std::thread t1([&q, &pushed] ()
	{
		for (int i = 0; i < N; ++i)
		{
			q.push_back(i);
			pushed += i;
		}
	});

	int32_t poped = 0;
	std::thread t2([&q, &poped] ()
	{
		int cnt = 0;
		while (cnt < N)
		{
			int32_t val;
			if (q.pop_front(val))
			{
				poped += val;
				++cnt;
			}
		}
	});

	t1.join();
	t2.join();

	PERF_CHECK(pushed == poped, "error: pushed = %d, poped = %d", pushed, poped);
}

REGISTER_PERF_CASE(mem_order, non_atomic     , (test<false, std::memory_order_relaxed, std::memory_order_relaxed>), M);
REGISTER_PERF_CASE(mem_order, relaxed_relaxed, (test<true,  std::memory_order_relaxed, std::memory_order_relaxed>), M);
REGISTER_PERF_CASE(mem_order, consume_release, (test<true,  std::memory_order_consume, std::memory_order_release>), M);
REGISTER_PERF_CASE(mem_order, acquire_release, (test<true,  std::memory_order_acquire, std::memory_order_release>), M);
REGISTER_PERF_CASE(mem_order, seq_cst_seq_cst, (test<true,  std::memory_order_seq_cst, std::memory_order_seq_cst>), M);

