#include <mm/memblock.h>

#include "init.h"

void arm64_memblock_init(void)
{
    uint64_t base;
    uint64_t size;


    base = 0x50000000;
    size = 0x20000000;

    // add total memory to manager by memblock
    // 0x50000000 - 0x70000000
    memblock_add(base, size);

    // TODO: add kernel image reserve
}