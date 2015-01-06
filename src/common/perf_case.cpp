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


perf_lab::perf_lab()
	: verbose_type_(msg_err)
	, verbose_count_(10)
{

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

int perf_lab::enable_perf_suite(const std::string& suite, bool enable)
{
	instance().disabled_[suite] = !enable;
	return instance().disabled_.size();
}

int perf_lab::set_perf_suite_initializer( const std::string& suite, perf_func func )
{
	instance().initializers_[suite] = func;
	return instance().initializers_.size();
}

int perf_lab::set_perf_suite_finisher( const std::string& suite, perf_func func )
{
	instance().finishers_[suite] = func;
	return instance().finishers_.size();
}

void perf_lab::add_msg(perf_msg_t type, std::string&& msg)
{
	instance().msgs_[type].emplace_back(std::move(msg));
}

void perf_lab::set_verbose(perf_msg_t type, int count)
{
	instance().verbose_type_ = type;
	instance().verbose_count_ = count;
}

std::string perf_lab::run()
{
	std::ostringstream oss;

	for (auto& suite : instance().perf_suites_)
	{
		if (instance().disabled_[suite.first]) continue;

		// initialize
		auto init = instance().initializers_.find(suite.first);
		if (init != instance().initializers_.end())
		{
			(init->second)();
		}

		oss << "[" << suite.first << "]" << std::endl;

		// run cases
		for (auto& cs : suite.second)
		{
			oss << cs.run() << std::endl;

			// output messages collected during this case
			int count = 0;
			for (int i = 0; i <= instance().verbose_type_; ++i)
			{
				for (auto& err : instance().msgs_[i])
				{
					if (count == instance().verbose_count_)
					{
						oss << cs.name() << ": ... ..." << std::endl;
						break;
					}
					oss << cs.name() << ": "<< err << std::endl;
					++count;
				}
				if (count == instance().verbose_count_) break;
			}

			// clear messages
			for (int i = 0; i < msg_max; ++i)
			{
				instance().msgs_[i].clear();
			}
		}
		oss << std::endl;

		// finish
		auto fini = instance().finishers_.find(suite.first);
		if (fini != instance().finishers_.end())
		{
			(fini->second)();
		}
	}
	return oss.str();
}


