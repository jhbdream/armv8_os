#ifndef _ASM_PGTABLE_H
#define _ASM_PGTABLE_H

#include <asm/pgtable_type.h>
#include <asm/pgtable_prot.h>
#include <asm/fixmap.h>
#include <ee/pgtable.h>

#define pgd_none(pgd) (!pgd_val(pgd))
#define pud_none(pud) (!pud_val(pud))
#define pmd_none(pmd) (!pmd_val(pmd))
#define pte_none(pte) (!pte_val(pte))

#define __pte_to_phys(pte)  (pte_val(pte) & PTE_ADDR_MASK)
#define __phys_to_pte_val(phys) (phys)
#define pte_pfn(pte)        (__pte_to_phys(pte) >> PAGE_SHIFT)
#define pfn_pte(pfn, prot)    \
        __pte(__phys_to_pte_val(pfn << PAGE_SHIFT) | pgprot_val(prot))


static inline pte_t pgd_pte(pgd_t pgd)
{
    return __pte(pgd_val(pgd));
}

static inline pte_t pud_pte(pud_t pud)
{
    return __pte(pud_val(pud));
}

static inline pud_t pte_pud(pte_t pte)
{
    return __pud(pte_val(pte));
}

static inline pmd_t pud_pmd(pud_t pud)
{
    return __pmd(pud_val(pud));
}

static inline pte_t pmd_pte(pmd_t pmd)
{
    return __pte(pmd_val(pmd));
}

static inline pmd_t pte_pmd(pte_t pte)
{
    return __pmd(pte_val(pte));
}

#define __pgd_to_phys(pgd)  __pte_to_phys(pgd_pte(pgd))
#define __phys_to_pgd_val(phys) __phys_to_pte_val(phys)

/* Return a pointer with offset calculated */
#define __set_fixmap_offset(idx, phys, flags)               \
({                                  \
    unsigned long ________addr;                 \
    __set_fixmap(idx, phys, flags);                 \
    ________addr = fix_to_virt(idx) + ((phys) & (PAGE_SIZE - 1));   \
    ________addr;                           \
})

#define set_fixmap_offset(idx, phys) \
    __set_fixmap_offset(idx, phys, PAGE_KERNEL)


#define pud_offset_phys(dir, addr) (__pgd_to_phys((*dir)) + pud_index(addr) * sizeof(pud_t))

#define pgd_set_fixmap(addr)        ((pgd_t *)set_fixmap_offset(FIX_PGD, addr))
#define pud_set_fixmap(addr)        ((pud_t *)set_fixmap_offset(FIX_PUD, addr))
#define pmd_set_fixmap(addr)        ((pmd_t *)set_fixmap_offset(FIX_PMD, addr))
#define pte_set_fixmap(addr)        ((pte_t *)set_fixmap_offset(FIX_PTE, addr))

#endif