#ifndef _ASM_PGTABLE_H
#define _ASM_PGTABLE_H

#include <asm/pgtable_type.h>

#define pgd_none(pgd) (!pgd_val(pgd))
#define pud_none(pud) (!pud_val(pud))
#define pmd_none(pmd) (!pmd_val(pmd))
#define pte_none(pte) (!pte_val(pte))

#endif