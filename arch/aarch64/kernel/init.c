#include <mm/memblock.h>
#include "init.h"
#include "asm/memory.h"

s64 memory_strart = -1;

void arm64_memblock_init(void)
{
    uint64_t base;
    uint64_t size;

    base = 0x40080000;
    size = 0x20000000;

    // add total memory to manager by memblock
    // 0x50000000 - 0x70000000
    memblock_add(base, size);
    memory_strart = base;

    // TODO: add kernel image reserve
    phys_addr_t kimage_start;
    phys_addr_t kimage_end;

    extern unsigned long __kimage_start[], __kimage_end[];

    kimage_start = __pa_symbol(__kimage_start);
    kimage_end = __pa_symbol(__kimage_end);

    memblock_reserve(kimage_start, kimage_end - kimage_start);
}