#include "init.h"
#include <asm/mmu.h>
#include <stdint.h>
#include <asm/pgtable_type.h>
#include <asm/pgtable_prot.h>
#include <mm/memblock.h>
#include <driver/console.h>
#include <mm/page_alloc.h>

extern pgd_t __init_pg_dir_start[512];

void setup_arch(void)
{
    arm64_memblock_init();

    early_fixmap_init((pgd_t *)__init_pg_dir_start);
    create_pgd_mapping((pgd_t *)__init_pg_dir_start, 0x09000000, 0xFFFF100000000000,
                0x1000, PAGE_DEVICE, early_pgtable_alloc, 0x00);

    paging_init();

#if 0
 	extern int arm_gicv3_interrupu_init(void *dist_base, void *rdist_base, uint32_t nr_redist_regions);
	// for qemu hardware address
	arm_gicv3_interrupu_init((void *)0x08000000, (void *)0x080A0000, 1);

	extern void aarch64_timer_init(void);
	aarch64_timer_init();
	local_irq_enable();
#endif
}
