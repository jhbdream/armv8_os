#include <mm/memblock.h>
#include "init.h"
#include "asm/memory.h"
#include <driver/of.h>

s64 memory_start = -1;

void arm64_memblock_init(void)
{
	/* scan mem node from dts && add mem to memblock reigon */
	of_fdt_scan(early_init_dt_scan_memory, NULL);
	memory_start = memblock_start_of_DRAM();

	// TODO: add kernel image reserve
	phys_addr_t kimage_start;
	phys_addr_t kimage_end;

	extern unsigned long __kimage_start[], __kimage_end[];

	kimage_start = __pa_symbol(__kimage_start);
	kimage_end = __pa_symbol(__kimage_end);

	memblock_reserve(kimage_start, kimage_end - kimage_start);
}
