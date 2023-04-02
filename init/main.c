#include <printk.h>
#include <driver/console.h>
#include <stdint.h>
#include <config.h>
#include <ee/irqflags.h>
#include <ee/init.h>
#include <io.h>
#include <driver/interrupt.h>
#include <mm/memblock.h>
#include <mm/page_alloc.h>
#include <config/version.h>

void start_kernel(void)
{
	local_irq_disable();

	setup_arch();

	console_init();

	printk("VERSION: %s\n", EEOS_VERSION_STR);

	memblock_dump_all();

	u64 base = memblock_start_of_DRAM();
	u64 size = memblock_end_of_DRAM() - memblock_start_of_DRAM();
	vmemmap_page_init(base, base + size);

	buddy_zone_init();
	free_memory_core();

#if 0
   extern void buddy_page_test(void);
   buddy_page_test();

   extern void slob_test(void);
   slob_test();
#endif

#ifdef CONFIG_FLAT_TEST
	extern void fdt_test(void);
	fdt_test();
#endif

	printk("hello kernel!\n");

	int vmalloc_test(void);
	vmalloc_test();

	for (;;)
		;
}
