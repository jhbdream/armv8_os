#ifndef __SLAB_H__
#define __SLAB_H__

#include <eeos/types.h>

extern void *__kmalloc(size_t size);
extern void  __kfree(const void *block);

static inline void *kmalloc(size_t size)
{
    return __kmalloc(size);
}

static inline void kfree(const void *block)
{
    __kfree(block);
}

#endif
