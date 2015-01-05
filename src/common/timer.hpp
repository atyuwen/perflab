#pragma once

#if defined(_MSC_VER)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class timer
{
public:
	timer()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		periodicity_ = 1.0 / frequency.QuadPart;
	}

	void start()
	{
		QueryPerformanceCounter(&counter_start_);
	}

	void stop()
	{
		QueryPerformanceCounter(&counter_stop_);
	}

	float duration()
	{
		double seconds = (counter_stop_.QuadPart - counter_start_.QuadPart) * periodicity_;
		return static_cast<float>(seconds * 1000);
	}

private:
	double periodicity_;
	LARGE_INTEGER counter_start_;
	LARGE_INTEGER counter_stop_;
};

#else

#include <time.h>
static inline void GetCurrentTimeInSeconds(double* t)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	*t = static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1e9;
}

class timer
{
public:
	timer()
	{
	}

	void start()
	{
		GetCurrentTimeInSeconds(&start_);
	}

	void stop()
	{
		GetCurrentTimeInSeconds(&stop_);
	}

	float duration()
	{
		double seconds = stop_ - start_;
		return static_cast<float>(seconds * 1000);
	}

private:
	double start_;
	double stop_;
};

#endif
