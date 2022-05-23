#ifndef _EE_PGTABLE_H
#define _EE_PGTABLE_H

#include <asm/pgtable_type.h>
#include <asm/pgtable_hwdef.h>

#define pgd_index(a)  (((a) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define pud_index(a)  (((a) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define pmd_index(a)  (((a) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define pte_index(a)  (((a) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

static inline pgd_t *pgd_offset_pgd(pgd_t *pgd, unsigned long addr)
{
    return (pgd + pgd_index(addr));
}

static inline pud_t *pud_offset_pud(pud_t *pud, unsigned long addr)
{
    return (pud + pud_index(addr));
}

static inline pmd_t *pmd_offset_pmd(pmd_t *pmd, unsigned long addr)
{
    return (pmd + pmd_index(addr));
}

static inline pte_t *pte_offset_pte(pte_t *pte, unsigned long addr)
{
    return (pte + pte_index(addr));
}

/*
 * When walking page tables, get the address of the next boundary,
 * or the end address of the range if that comes earlier.  Although no
 * vma end wraps to 0, rounded up __boundary may wrap to 0 throughout.
 */
#ifndef pud_addr_end
#define pgd_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK;  \
    (__boundary - 1 < (end) - 1)? __boundary: (end);        \
})
#endif

#ifndef pud_addr_end
#define pud_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;  \
    (__boundary - 1 < (end) - 1)? __boundary: (end);        \
})
#endif

#ifndef pmd_addr_end
#define pmd_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;  \
    (__boundary - 1 < (end) - 1)? __boundary: (end);        \
})
#endif

#endif