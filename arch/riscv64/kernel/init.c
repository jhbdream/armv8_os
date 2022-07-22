#include <mm/memblock.h>
#include "asm/memory.h"
#include "type.h"

signed long memory_start;

void rv64_memblock_init(phys_addr_t start, phys_addr_t end)
{
    uint64_t base = start;
    uint64_t size = end - start;

    // add total memory to manager by memblock
    memblock_add(base, size);
    memory_start = base;

    phys_addr_t kimage_start;
    phys_addr_t kimage_end;

    extern unsigned long __kimage_start[], __kimage_end[];

    kimage_start = __pa_symbol(__kimage_start);
    kimage_end = __pa_symbol(__kimage_end);

    memblock_reserve(kimage_start, kimage_end - kimage_start);
}
