#ifndef _LINUX_MEMBLOCK_H
#define _LINUX_MEMBLOCK_H

#include <type.h>

struct memblock_region
{
    phys_addr_t base;
    phys_addr_t size;
};

struct memblock_type
{
    unsigned long cnt;
    unsigned long max;
    phys_addr_t total_size;
    struct memblock_region *regions;
    char *name;
};

struct memblock
{
    struct memblock_type memory;
    struct memblock_type reserved;
};

#define __for_each_mem_range_rev(i, type_a, type_b, p_start, p_end)                     \
    for (i = (u64)ULLONG_MAX, __next_mem_range_rev(&i, type_a, type_b, p_start, p_end); \
         i != (u64)ULLONG_MAX;                                                          \
         __next_mem_range_rev(&i, type_a, type_b, p_start, p_end))

/**
 * @brief 逆序遍历memblock (memory && !reserved)
 *
 */
#define for_each_free_mem_range_reverse(i, p_start, p_end) \
    __for_each_mem_range_rev(i, &memblock.memory, &memblock.reserved, p_start, p_end)

int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);
phys_addr_t memblock_alloc_range(phys_addr_t size, phys_addr_t align);
void *memblock_alloc(phys_addr_t size, phys_addr_t align);
int memblock_phys_free(phys_addr_t base, phys_addr_t size);
void memblock_free(void *ptr, size_t size);

#endif
