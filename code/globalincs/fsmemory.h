
#ifndef _FSMEMORY_H
#define _FSMEMORY_H

#include <new>

#include "compiler.h"

#ifdef HAS_CXX11_NOEXCEPT
#define SCP_NOEXCEPT noexcept
#else
#define SCP_NOEXCEPT
#endif

// throw
extern void * operator new (size_t size);

extern void operator delete (void *p) SCP_NOEXCEPT;

extern void * operator new [] (size_t size);

extern void operator delete[](void *p) SCP_NOEXCEPT;

// no-throw
extern void * operator new (size_t size, const std::nothrow_t&) SCP_NOEXCEPT;

extern void operator delete (void *p, const std::nothrow_t&) SCP_NOEXCEPT;

extern void * operator new[](size_t size, const std::nothrow_t&) SCP_NOEXCEPT;

extern void operator delete[](void *p, const std::nothrow_t&) SCP_NOEXCEPT;

#endif	// _FSMEMORY_H
