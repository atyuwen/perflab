
#pragma once

#ifdef _MSC_VER
#define ALIGN(n) __declspec(align(n))
#else
#define ALIGN(n) __attribute__ ((aligned(n)))
#endif

