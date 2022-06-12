#ifndef _LINUX_MEMBLOCK_H
#define _LINUX_MEMBLOCK_H

#include <type.h>
#include <limits.h>
#include <stddef.h>

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

extern struct memblock memblock;

void __next_mem_range(u64 *idx,
                          struct memblock_type *type_a,
                          struct memblock_type *type_b,
                          phys_addr_t *out_start,
                          phys_addr_t *out_end);

void __next_mem_range_rev(u64 *idx,
                          struct memblock_type *type_a,
                          struct memblock_type *type_b,
                          phys_addr_t *out_start,
                          phys_addr_t *out_end);
int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);
phys_addr_t memblock_phys_alloc_align(phys_addr_t size, phys_addr_t align);
void *memblock_alloc(phys_addr_t size, phys_addr_t align);
int memblock_phys_free(phys_addr_t base, phys_addr_t size);
void memblock_free(void *ptr, size_t size);
void memblock_dump_all(void);

#define __for_each_mem_range(i, type_a, type_b, p_start, p_end) \
    for (i = (u64)0, __next_mem_range(&i, type_a, type_b, p_start, p_end); \
         i != (u64)ULLONG_MAX;                                                          \
         __next_mem_range(&i, type_a, type_b, p_start, p_end))

#define __for_each_mem_range_rev(i, type_a, type_b, p_start, p_end)                     \
    for (i = (u64)ULLONG_MAX, __next_mem_range_rev(&i, type_a, type_b, p_start, p_end); \
         i != (u64)ULLONG_MAX;                                                          \
         __next_mem_range_rev(&i, type_a, type_b, p_start, p_end))

#define for_each_mem_range(i, p_start, p_end) \
    __for_each_mem_range(i, &memblock.memory, NULL, p_start, p_end)

#define for_each_mem_range_rev(i, p_start, p_end) \
    __for_each_mem_range_rev(i, &memblock.memory, NULL, p_start, p_end)

#define for_each_free_mem_range(i, p_start, p_end) \
    __for_each_mem_range(i, &memblock.memory, &memblock.reserved, p_start, p_end)

#define for_each_free_mem_range_reverse(i, p_start, p_end) \
    __for_each_mem_range_rev(i, &memblock.memory, &memblock.reserved, p_start, p_end)

#define for_each_reserved_mem_range(i, p_start, p_end) \
    __for_each_mem_range(i, &memblock.reserved, NULL, p_start, p_end)

#endif
