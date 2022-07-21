#ifndef _FIXMAP_H
#define _FIXMAP_H

#include <ee/sizes.h>
#include <asm/memory.h>
#include <asm/pgtable_hwdef.h>

enum fixed_addresses
{
    FIX_HOLE,
    FIX_EARLYCON_MEM_BASE,

    /*
     * Used for kernel page table creation, so unmapped memory may be used
     * for tables.
     */
    FIX_PTE,
    FIX_PMD,
    FIX_PUD,
    FIX_PGD,

    __end_of_fixed_addresses
};

#define FIXADDR_SIZE (__end_of_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_START (FIXADDR_TOP - FIXADDR_SIZE)

#include <asm-generic/fixmap.h>

#endif
