#include "driver/console.h"
#include "printk.h"
#include <ee/irqflags.h>
#include <type.h>
#include <mm/memblock.h>
#include <mm/page_alloc.h>
#include <driver/of.h>

signed long memory_start;

void setup_arch(void)
{
	/* scan mem node from dts && add mem to memblock reigon */
	of_fdt_scan(early_init_dt_scan_memory, NULL);

	/* init memory_start */
	memory_start = memblock_start_of_DRAM();

	extern unsigned long __kimage_start[], __kimage_end[];
	phys_addr_t kimage_start;
	phys_addr_t kimage_end;

	kimage_start = __pa_symbol(__kimage_start);
	kimage_end = __pa_symbol(__kimage_end);
	memblock_reserve(kimage_start, kimage_end - kimage_start);

	extern void setup_vm_final(void);
	setup_vm_final();

#if 0
	extern void riscv_plic_init(void);
	riscv_plic_init();
	local_irq_enable();
#endif

#if 0
	extern void riscv_timer_init(void);
	riscv_timer_init();
#endif
}
