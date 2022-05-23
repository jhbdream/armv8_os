#ifndef _ASM_PGALLOC_H
#define _ASM_PGALLOC_H

#include <asm/pgtable_type.h>

static inline void __pgd_populate(pgd_t *pgd, phys_addr_t pudp, pgdval_t prot)
{
    pgd_t val = __pgd(pudp | prot);
    *pgd = val;
}

static inline void __pud_populate(pud_t *pud, phys_addr_t pmdp, pudval_t prot)
{
    pud_t val = __pud(pmdp | prot);
    *pud = val;
}

static inline void __pmd_populate(pmd_t *pmd, phys_addr_t ptep, pmdval_t prot)
{
    pmd_t val = __pmd(ptep | prot);
    *pmd = val;
}

static inline void __pte_populate(pte_t *pte, phys_addr_t page, pteval_t prot)
{
    pte_t val = __pte(page | prot);
    *pte = val;
}


#endif