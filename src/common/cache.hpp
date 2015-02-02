
#pragma once

enum invalidate_mode
{
	invalidate_write_back,
	invalidate_discard,
};

#if defined(_MSC_VER)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <emmintrin.h>

// NOTE: this function should be called on ring0
static inline void invalide_cache(invalidate_mode mode)
{
	if (mode == invalidate_write_back)
	{
		__wbinvd();
	}
	else
	{
		__asm invd;
	}
}

static inline void flush_cache(void* begin, void* end)
{
	const int cache_line = 64;
	char* p = (char*)((int)begin & (~(cache_line - 1)));
	while (p < end)
	{
		_mm_clflush(p);
		p += cache_line;
	}
}

static inline void init_large_page()
{
	HANDLE      hToken;
	TOKEN_PRIVILEGES tp;
	BOOL       status;
	DWORD      error;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, TEXT("SeLockMemoryPrivilege"), &tp.Privileges[0].Luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	status = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	error = GetLastError();
	CloseHandle(hToken);
}

static inline void* alloc_large_page(int size)
{
	return VirtualAlloc(0, size, MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
}

static inline void free_large_page(void* data)
{
	VirtualFree(data, 0, MEM_RELEASE);
}

#else

// TODO
static inline void invalide_cache(invalidate_mode mode)
{

}

static inline void flush_cache(void* begin, void* end)
{

}

static inline void init_large_page()
{

}

static inline void* alloc_large_page(int size)
{
	return new char[size];
}

static inline void free_large_page(void* data)
{
	delete [] (char*)(data);
}

#endif



