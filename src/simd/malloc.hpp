#pragma once

#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif

namespace simd
{
	static inline void* aligned_malloc(size_t size, size_t align)
	{
#if defined(_MSC_VER)
		return _aligned_malloc(size, align);
#else 
		return _mm_malloc(size, align);
#endif
	}

	static inline void aligned_free(void *ptr)
	{
#if defined(_MSC_VER)
		_aligned_free(ptr);
#else 
		_mm_free(ptr);
#endif
	}
}

