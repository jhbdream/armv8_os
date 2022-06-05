#include "init.h"
#include <asm/mmu.h>
#include <stdint.h>
#include <asm/pgtable_type.h>
#include <pgtable_prot.h>

extern pgd_t __init_pg_dir_start[512];

void setup_arch(void)
{

    arm64_memblock_init();

    early_fixmap_init((pgd_t *)__init_pg_dir_start);

    paging_init();

    create_pgd_mapping((pgd_t *)__init_pg_dir_start, 0x09000000, 0xFFFF000080000000,
                0x1000, __pgprot(PROT_DEVICE_nGnRnE), early_pgtable_alloc, 0x00);
}
