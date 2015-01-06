#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>

typedef std::function<void()> perf_func;

class perf_case
{
public:
	perf_case(const std::string&name, perf_func func, int repeat)
		: name_(name)
		, func_(func)
		, repeat_(repeat)
	{}

	const std::string& name() const {return name_;}

	std::string run();

private:
	std::string            name_;
	perf_func              func_;
	int                    repeat_;
};

typedef std::vector<perf_case> perf_suite;

enum perf_msg_t : int
{
	msg_none = -1,

	msg_err,
	msg_debug,
	msg_info,

	msg_max,
};

class perf_lab
{
public:
	static int enable_perf_suite(const std::string& suite, bool enable);

	static int set_perf_suite_initializer(const std::string& suite, perf_func func);

	static int set_perf_suite_finisher(const std::string& suite, perf_func func);

	static int add_perf_case(const std::string& suite, const std::string&name, std::function<void()> func, int repeat);

	static void add_msg(perf_msg_t type, std::string&& msg);

	static void set_verbose(perf_msg_t type, int count);

	static std::string run();

private:
	perf_lab();

	static perf_lab& instance();

private:
	std::map<std::string, perf_suite>  perf_suites_;
	std::map<std::string, bool>        disabled_;
	std::map<std::string, perf_func>   initializers_;
	std::map<std::string, perf_func>   finishers_;
	std::vector<std::string>           msgs_[msg_max];
	perf_msg_t                         verbose_type_;
	int                                verbose_count_;
};

#if !defined(_MSC_VER)
	#define sprintf_s sprintf
#endif

namespace perf_xxx
{
	struct register_perf_suite_helper
	{
	public:
		register_perf_suite_helper(const std::string& suite)
			: suite_(suite) {}

		int operator() ()
		{
			return 0;
		}

		int operator() (perf_func initializer)
		{
			return perf_lab::set_perf_suite_initializer(suite_, initializer);
		}

		int operator() (perf_func initializer, perf_func finisher)
		{
			return perf_lab::set_perf_suite_initializer(suite_, initializer)
			     | perf_lab::set_perf_suite_finisher(suite_, finisher);
		}

	private:
		std::string suite_;
	};
}

#define REGISTER_PERF_SUITE(suite, enable, /* optional initializer, optional finisher */...) \
	int suite_##suite = perf_lab::enable_perf_suite(#suite, enable) \
                      | perf_xxx::register_perf_suite_helper(#suite)(__VA_ARGS__);

#define REGISTER_PERF_CASE(suite, name, func, repeat) \
	int case_##suite##_##name = perf_lab::add_perf_case(#suite, #name, func, repeat);

#define PERF_CHECK(cond, msg, ...) \
	do if (!(cond)) \
	{ \
		char buffer[256]; \
		sprintf_s(buffer, msg, __VA_ARGS__); \
		perf_lab::add_msg(msg_err, std::string(buffer)); \
	} while(0)

#define PERF_MSG(type, msg, ...) \
	do \
	{ \
		char buffer[256]; \
		sprintf_s(buffer, msg, __VA_ARGS__); \
		perf_lab::add_msg(type, std::string(buffer)); \
	} while(0)

