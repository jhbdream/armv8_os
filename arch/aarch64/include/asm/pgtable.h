#ifndef _ASM_PGTABLE_H
#define _ASM_PGTABLE_H

#include <asm/pgtable_hwdef.h>
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

#define pud_pfn(pud)        ((__pud_to_phys(pud) & PUD_MASK) >> PAGE_SHIFT)
#define __phys_to_pud_val(phys) __phys_to_pte_val(phys)
#define pfn_pud(pfn,prot)   __pud(__phys_to_pud_val((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot))

#define pmd_pfn(pmd)        ((__pmd_to_phys(pmd) & PMD_MASK) >> PAGE_SHIFT)
#define __phys_to_pmd_val(phys) __phys_to_pte_val(phys)
#define pfn_pmd(pfn,prot)   __pmd(__phys_to_pmd_val((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot))
#define mk_pmd(page,prot)   pfn_pmd(page_to_pfn(page),prot)

#define __pgd_to_phys(pgd)  __pte_to_phys(pgd_pte(pgd))
#define __pud_to_phys(pud)  __pte_to_phys(pud_pte(pud))
#define __pmd_to_phys(pmd)  __pte_to_phys(pmd_pte(pmd))

#define __phys_to_pgd_val(phys) __phys_to_pte_val(phys)

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

static inline pgprot_t mk_pud_sect_prot(pgprot_t prot)
{
    return (__pgprot( (pgprot_val(prot) & ~PUD_TABLE_BIT) | PUD_TYPE_SECT));
}

static inline pgprot_t mk_pmd_sect_prot(pgprot_t prot)
{
    return (__pgprot( (pgprot_val(prot) & ~PMD_TABLE_BIT) | PMD_TYPE_SECT));
}

static inline phys_addr_t pgd_page_paddr(pgd_t pgd)
{
    return __pgd_to_phys(pgd);
}

static inline phys_addr_t pud_page_paddr(pud_t pud)
{
    return __pud_to_phys(pud);
}

static inline phys_addr_t pmd_page_paddr(pmd_t pmd)
{
    return __pmd_to_phys(pmd);
}

static inline phys_addr_t pte_page_paddr(pte_t pte)
{
    return __pte_to_phys(pte);
}

#define pud_offset_phys(dir, addr) (__pgd_to_phys((*dir)) + pud_index(addr) * sizeof(pud_t))
#define pmd_offset_phys(dir, addr) (__pud_to_phys((*dir)) + pmd_index(addr) * sizeof(pmd_t))
#define pte_offset_phys(dir, addr) (__pmd_to_phys((*dir)) + pte_index(addr) * sizeof(pte_t))

#define pud_set_fixmap(dir, addr)    ((pud_t *)set_fixmap_offset(FIX_PUD, pud_offset_phys(dir, addr)))
#define pmd_set_fixmap(dir, addr)    ((pmd_t *)set_fixmap_offset(FIX_PMD, pmd_offset_phys(dir, addr)))
#define pte_set_fixmap(dir, addr)    ((pte_t *)set_fixmap_offset(FIX_PTE, pte_offset_phys(dir, addr)))

/*
 * The following only work if pte_present(). Undefined behaviour otherwise.
 */
#define pte_present(pte)    (!!(pte_val(pte) & (PTE_VALID | PTE_PROT_NONE)))
#define pte_young(pte)      (!!(pte_val(pte) & PTE_AF))
#define pte_special(pte)    (!!(pte_val(pte) & PTE_SPECIAL))
#define pte_write(pte)      (!!(pte_val(pte) & PTE_WRITE))
#define pte_user_exec(pte)  (!(pte_val(pte) & PTE_UXN))
#define pte_cont(pte)       (!!(pte_val(pte) & PTE_CONT))
#define pte_devmap(pte)     (!!(pte_val(pte) & PTE_DEVMAP))
#define pte_tagged(pte)     ((pte_val(pte) & PTE_ATTRINDX_MASK) == \
        PTE_ATTRINDX(MT_NORMAL_TAGGED))

#define pgd_present(pgd) pgd_val(pgd)
#define pud_present(pud) pte_present(pud_pte(pud))
#define pmd_present(pmd) pte_present(pmd_pte(pmd))

#endif
