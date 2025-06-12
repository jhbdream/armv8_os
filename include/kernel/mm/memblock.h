#ifndef _LINUX_MEMBLOCK_H
#define _LINUX_MEMBLOCK_H

#include <stddef.h>
#include <eeos/types.h>

int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);

void *memblock_alloc(phys_addr_t size, phys_addr_t align);
void  memblock_free(void *ptr, size_t size);

void free_memory_core(void);

#endif
