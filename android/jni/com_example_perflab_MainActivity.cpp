#include "com_example_perflab_MainActivity.h"
#include "common/perf_case.hpp"

extern "C"
{
	jstring Java_com_example_perflab_MainActivity_stringFromJNI(JNIEnv* env, jobject obj)
	{
		std::string str = perf_lab::run();
		return env->NewStringUTF(str.c_str());
	}
}

