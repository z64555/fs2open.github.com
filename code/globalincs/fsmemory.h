
#ifndef _FSMEMORY_H
#define _FSMEMORY_H

#include <new>

#include "globalincs/scp_defines.h"

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
