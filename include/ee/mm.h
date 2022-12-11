#ifndef _EE_MM_H
#define _EE_MM_H

#include <eeos.h>
#include "asm/pgtable.h"
#include "asm/pgtable_type.h"
#include "pgtable.h"

int __pud_alloc(pgd_t *pgd, unsigned long address);

static inline pud_t *pud_alloc(pgd_t *pgd, unsigned long address)
{
  return ((pgd_none(*pgd)) && __pud_alloc(pgd, address))
             ? NULL
             : pud_offset(pgd, address);
}

int __pmd_alloc(pud_t *pud, unsigned long address);

static inline pmd_t *pmd_alloc(pud_t *pud, unsigned long address)
{
  return ((pud_none(*pud)) && __pmd_alloc(pud, address))
             ? NULL
             : pmd_offset(pud, address);
}

int __pte_alloc(pmd_t *pmd, unsigned long address);
static inline pte_t *pte_alloc(pmd_t *pmd, unsigned long address)
{
  return ((pmd_none(*pmd)) && __pte_alloc(pmd, address))
             ? NULL
             : pte_offset(pmd, address);
}

#endif
