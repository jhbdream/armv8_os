#ifndef _ASM_PGTABLE_H
#define _ASM_PGTABLE_H

#include <asm/pgtable_type.h>
#include <asm/pgtable_prot.h>
#include <asm/fixmap.h>

#define pgd_none(pgd) (!pgd_val(pgd))
#define pud_none(pud) (!pud_val(pud))
#define pmd_none(pmd) (!pmd_val(pmd))
#define pte_none(pte) (!pte_val(pte))

#define __pte_to_phys(pte)  (pte_val(pte) & PTE_ADDR_MASK)
#define __phys_to_pte_val(phys) (phys)
#define pte_pfn(pte)        (__pte_to_phys(pte) >> PAGE_SHIFT)
#define pfn_pte(pfn, prot)    \
        __pte(__phys_to_pte_val(pfn << PAGE_SHIFT) | pgprot_val(prot))


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

#define pgd_set_fixmap(addr)        ((pgd_t *)set_fixmap_offset(FIX_PGD, addr))
#define pud_set_fixmap(addr)        ((pud_t *)set_fixmap_offset(FIX_PUD, addr))
#define pmd_set_fixmap(addr)        ((pmd_t *)set_fixmap_offset(FIX_PMD, addr))
#define pte_set_fixmap(addr)        ((pte_t *)set_fixmap_offset(FIX_PTE, addr))

#endif