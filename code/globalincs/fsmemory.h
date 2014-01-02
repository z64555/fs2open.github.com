
#ifndef _FSMEMORY_H
#define _FSMEMORY_H

#include <new>

#include "compiler.h"

#ifdef HAS_CXX11_NOEXCEPT
	#define NOTHROW noexcept
#else
	#define NOTHROW throw()
#endif // HAS_CXX11_NOEXCEPT

// throw
void * operator new (size_t size);

void operator delete (void *p) NOTHROW;

void * operator new [] (size_t size);

void operator delete [] (void *p) NOTHROW;

// no-throw
void * operator new (size_t size, const std::nothrow_t&) NOTHROW;

void operator delete (void *p, const std::nothrow_t&) NOTHROW;

void * operator new [] (size_t size, const std::nothrow_t&) NOTHROW;

void operator delete [] (void *p, const std::nothrow_t&) NOTHROW;

#endif	// _FSMEMORY_H
