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
}

struct memblock
{
    struct memblock_type memory;
    struct memblock_type reserved;
}

#endif
