#ifndef _EE_PGTABLE_H
#define _EE_PGTABLE_H

#include <asm/pgtable_type.h>
#include <asm/pgtable_hwdef.h>

#define pgd_index(a)  (((a) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

static inline pgd_t *pgd_offset_pgd(pgd_t *pgd, unsigned long addr)
{
    return (pgd + pgd_index(addr));
}



#endif