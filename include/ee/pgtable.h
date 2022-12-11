#ifndef _EE_PGTABLE_H
#define _EE_PGTABLE_H

#include "asm/pgtable_hwdef.h"
#include <asm/pgtable.h>
#include <asm/pgalloc.h>

extern pgd_t swapper_pg_dir[PTRS_PER_PGD];

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

/* 根据虚拟地址查找内核页表 PGD 表项 */
/* swapper_pg_dir 是内核页表 */
static inline pgd_t *pgd_offset_k(unsigned long addr)
{
    return ((pgd_t *)swapper_pg_dir + pgd_index(addr));
}

#define pgd_page_vaddr(pgd) ((unsigned long) __va((unsigned long)pgd_val(pgd) & PTE_ADDR_MASK))
#define pud_page_vaddr(pud) ((unsigned long) __va((unsigned long)pud_val(pud) & PTE_ADDR_MASK))
#define pmd_page_vaddr(pmd) ((unsigned long) __va((unsigned long)pmd_val(pmd) & PTE_ADDR_MASK))

static inline pud_t *pud_offset(pgd_t *pgd, unsigned long address)
{
    return (pud_t *)pgd_page_vaddr(*(pgd)) + pud_index(address);
}

static inline pmd_t *pmd_offset(pud_t *pud, unsigned long address)
{
    return (pmd_t *)pud_page_vaddr(*(pud)) + pmd_index(address);
}

static inline pte_t *pte_offset(pmd_t *pmd, unsigned long address)
{
    return (pte_t *)pmd_page_vaddr(*(pmd)) + pte_index(address);
}

/*
 * When walking page tables, get the address of the next boundary,
 * or the end address of the range if that comes earlier.  Although no
 * vma end wraps to 0, rounded up __boundary may wrap to 0 throughout.
 */

/* 根据 end 地址获取下一个页表项的起始地址
 * 可能是下一级页表地址 也可能是end的地址
 * 当 end - addr 小于当前页表单位大小时，下一项就是end的地址
 */
#ifndef pud_addr_end
#define pgd_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK;  \
    (__boundary - 1 < (end) - 1) ? __boundary : (end);        \
})
#endif

#ifndef pud_addr_end
#define pud_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;  \
    (__boundary - 1 < (end) - 1) ? __boundary : (end);        \
})
#endif

#ifndef pmd_addr_end
#define pmd_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;  \
    (__boundary - 1 < (end) - 1) ? __boundary : (end);        \
})
#endif

#endif
