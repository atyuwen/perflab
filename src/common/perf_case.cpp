#include "common/timer.hpp"
#include <vector>
#include <numeric>
#include <algorithm>
#include <sstream>

#include "perf_case.hpp"

std::string perf_case::run()
{
	timer t;
	std::vector<float> durations(repeat_);
	for (int i = 0; i < repeat_; ++i)
	{
		t.start();
		func_();
		t.stop();
		durations[i] = t.duration();
	}

	std::sort(durations.begin(), durations.end());
	float total = std::accumulate(durations.begin(), durations.end(), 0.0f);
	float mean = total / repeat_;

	float sqr_total = std::accumulate(durations.begin(), durations.end(), 0.0f, [](float acc, float x)
	{
		return acc + x * x;
	});

	float sdev = sqrt(sqr_total / repeat_ - mean * mean);

	char buffer[256] = {0};
	sprintf_s(buffer, "%s: mean=%5.2f, sdev=%5.2f, min=%5.2f, max=%5.2f, median=%5.2f",
		name_.c_str(), mean, sdev, durations.front(), durations.back(), durations[repeat_ / 2]);

	return std::string(buffer);
}

perf_lab& perf_lab::instance()
{
	static perf_lab inst;
	return inst;
}

int perf_lab::add_perf_case(const std::string& suite, const std::string&name, std::function<void()> func, int repeat)
{
	perf_suite& the_suite = instance().perf_suites_[suite];
	the_suite.emplace_back(name, func, repeat);
	return the_suite.size();
}

void perf_lab::error_msg(std::string&& err)
{
	instance().errs_.emplace_back(std::move(err));
}

std::string perf_lab::run()
{
	std::ostringstream oss;

	for (auto& suite : instance().perf_suites_)
	{
		oss << "[" << suite.first << "]" << std::endl;

		for (auto& cs : suite.second)
		{
			instance().errs_.clear();

			oss << cs.run() << std::endl;

			// print errors
			for (auto& err : instance().errs_)
			{
				oss << cs.name() << ": "<< err << std::endl;
			}
		}
		oss << std::endl;
	}
	return oss.str();
}

