#include <mm/memblock.h>

#include "init.h"

void arm64_memblock_init(void)
{
    uint64_t base;
    uint64_t size;

    base = 0x40000000;
    size = 0x40000000;

    // add total memory to manager by memblock
    memblock_add(base, size);

    // TODO: add kernel image reserve
}