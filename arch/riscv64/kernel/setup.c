#include "driver/console.h"
#include "printk.h"
#include <ee/irqflags.h>
#include <type.h>
#include <mm/memblock.h>
#include <mm/page_alloc.h>

void setup_arch(void)
{
	extern void rv64_memblock_init(phys_addr_t start, phys_addr_t end);
	rv64_memblock_init(0x80000000, 0xc0000000);

	extern void setup_vm_final(void);
	setup_vm_final();

#if 0
	extern void riscv_plic_init(void);
	riscv_plic_init();
	local_irq_enable();
#endif

	console_init();

	printk("hello world\n");

    memblock_dump_all();

	u64 base = 0x80000000;
    u64 size = 0x40000000;

	vmemmap_page_init(base, base + size);
	buddy_zone_init();
	free_memory_core();

	extern void buddy_page_test(void);
	buddy_page_test();

	while(1);

	extern void riscv_timer_init(void);
	riscv_timer_init();
}
