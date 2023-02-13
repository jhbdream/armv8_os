#ifndef __SLAB_H__
#define __SLAB_H__

#include <type.h>
#include <compiler_types.h>

extern void *__kmalloc(size_t size);
extern void __kfree(const void *block);

static __always_inline void *kmalloc(size_t size)
{
	return __kmalloc(size);
}

static __always_inline void kfree(const void *block)
{
	__kfree(block);
}

#endif
