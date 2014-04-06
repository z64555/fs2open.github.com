
#include "globalincs/pstypes.h"


// throw
void * operator new (size_t size)
{
	void *p = vm_malloc_q(size);

	if ( !p ) {
		throw std::bad_alloc();
	}

	return p;
}

void operator delete (void *p) SCP_NOEXCEPT
{
	vm_free(p);
}

void * operator new [] (size_t size)
{
	void *p = vm_malloc_q(size);

	if ( !p ) {
		throw std::bad_alloc();
	}

	return p;
}

void operator delete[](void *p) SCP_NOEXCEPT
{
	vm_free(p);
}

// no-throw
void * operator new (size_t size, const std::nothrow_t&) SCP_NOEXCEPT
{
	return vm_malloc_q(size);
}

void operator delete (void *p, const std::nothrow_t&) SCP_NOEXCEPT
{
	vm_free(p);
}

void * operator new[](size_t size, const std::nothrow_t&) SCP_NOEXCEPT
{
	return vm_malloc_q(size);
}

void operator delete[](void *p, const std::nothrow_t&) SCP_NOEXCEPT
{
	vm_free(p);
}
