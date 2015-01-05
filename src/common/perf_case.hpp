#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>

#if !defined(_MSC_VER)
	#define sprintf_s sprintf
#endif

#define REGISTER_PERF_CASE(suite, name, func, repeat) \
	int token_##suite##_##name = perf_lab::add_perf_case(#suite, #name, func, repeat);

#define PERF_CHECK(cond, msg, ...) \
	if (!(cond)) \
	{ \
		char buffer[256]; \
		sprintf_s(buffer, msg, __VA_ARGS__); \
		perf_lab::error_msg(buffer); \
	}

class perf_case
{
public:
	perf_case(const std::string&name, std::function<void()> func, int repeat)
		: name_(name)
		, func_(func)
		, repeat_(repeat)
	{}

	const std::string& name() const {return name_;}

	std::string run();

private:
	std::string            name_;
	std::function<void()>  func_;
	int                    repeat_;
};

typedef std::vector<perf_case> perf_suite;

class perf_lab
{
public:
	static int add_perf_case(const std::string& suite, const std::string&name, std::function<void()> func, int repeat);

	static void error_msg(std::string&& err);

	static std::string run();

private:
	perf_lab() {}

	static perf_lab& instance();

private:
	std::map<std::string, perf_suite>  perf_suites_;
	std::vector<std::string>           errs_;
};

