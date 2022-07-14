#ifndef _FIXMAP_H
#define _FIXMAP_H

#include <asm/memory.h>
#include <ee/sizes.h>
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

#define __fix_to_virt(x)    (FIXADDR_TOP - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)    ((FIXADDR_TOP - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

static inline unsigned long fix_to_virt(const unsigned int idx)
{
    return __fix_to_virt(idx);
}

static inline unsigned long virt_to_fix(const unsigned long vaddr)
{
    return __virt_to_fix(vaddr);
}

#endif
