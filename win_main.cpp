#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "common/perf_case.hpp"

int main()
{
	//perf_lab::set_verbose(msg_info, 100);
	std::string res = perf_lab::run();
	std::cout << res << std::endl;

	SYSTEMTIME sys_time;
	::GetLocalTime(&sys_time);

	char timestamp[256] = {0};
	sprintf_s(timestamp, TEXT("%d/%d/%d %02d:%02d:%02d"),
		sys_time.wYear, sys_time.wMonth, sys_time.wDay,
		sys_time.wHour, sys_time.wMinute, sys_time.wSecond);

	std::ofstream ofs("log.txt", std::ios::out | std::ios::app);
	ofs << std::string(64, '-') << timestamp << std::endl;
	ofs << res << std::endl;

	system("pause");
}

