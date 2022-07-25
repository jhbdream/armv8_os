#include "init.h"
#include <asm/mmu.h>
#include <stdint.h>
#include <asm/pgtable_type.h>
#include <asm/pgtable_prot.h>
#include <mm/memblock.h>
#include <driver/console.h>

extern pgd_t __init_pg_dir_start[512];

void setup_arch(void)
{
	console_init();
    arm64_memblock_init();

    early_fixmap_init((pgd_t *)__init_pg_dir_start);

    create_pgd_mapping((pgd_t *)__init_pg_dir_start, 0x09000000, 0xFFFF100000000000,
                0x1000, PAGE_DEVICE, early_pgtable_alloc, 0x00);

    paging_init();
    memblock_dump_all();

    u64 base = 0x40080000;
    u64 size = 0x20000000;

	extern int vmemmap_page_init(unsigned long mem_start, unsigned long mem_end);
	vmemmap_page_init(base, base + size);

	extern int buddy_init(void);
	buddy_init();

	extern void free_memory_core(void);
	free_memory_core();

	extern void buddy_page_test(void);
	buddy_page_test();

#if 0
 	extern int arm_gicv3_interrupu_init(void *dist_base, void *rdist_base, uint32_t nr_redist_regions);
	// for qemu hardware address
	arm_gicv3_interrupu_init((void *)0x08000000, (void *)0x080A0000, 1);

	extern void aarch64_timer_init(void);
	aarch64_timer_init();
	local_irq_enable();
#endif
}
