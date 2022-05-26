#include "init.h"
#include <asm/mmu.h>
#include <stdint.h>
#include <asm/pgtable_type.h>

extern pgd_t test_pg_dir[512];
extern uint64_t __init_pg_dir_start[512];

void setup_arch(void)
{

    arm64_memblock_init();

    early_fixmap_init((pgd_t *)__init_pg_dir_start);

    create_pgd_mapping((pgd_t *)test_pg_dir, 0x80000000, 0xFFFF800000000000,
                0x50000000, __pgprot(0x00), early_pgtable_alloc, 0x00);
}
