#ifndef _EE_PGTABLE_H
#define _EE_PGTABLE_H

#include <type.h>

/**
 * page table 相关定义 尽量实现通用
 * 默认只支持四级页表模式
 */

typedef u64 pgdval_t;
typedef u64 pudval_t;
typedef u64 pmdval_t;
typedef u64 pteval_t;
typedef u64 prot_val_t;

typedef struct {
	pgdval_t pgd;
} pgd_t;

typedef struct {
	pudval_t pud;
} pud_t;

typedef struct {
	pmdval_t pmd;
} pmd_t;

typedef struct {
	pteval_t pte;
} pte_t;

typedef struct {
	prot_val_t pgprot;
} pgprot_t;

#define pgd_val(x) ((x).pgd)
#define _pgd(x) ((pgd_t){ (x) })

#define pud_val(x) ((x).pud)
#define _pud(x) ((pud_t){ (x) })

#define pmd_val(x) ((x).pmd)
#define _pmd(x) ((pmd_t){ (x) })

#define pte_val(x) ((x).pte)
#define _pte(x) ((pte_t){ (x) })

#define pgprot_val(x) ((x).pgprot)
#define _pgprot(x) ((pgprot_t){ (x) })

/* 公共宏定义 */
/* config for 48bit va + 4 level page table */
#define PAGE_SHIFT (12)
#define PAGE_SIZE (_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PTE_ADDR_MASK (PAGE_MASK)
#define PTRS_PER_PTE (512)

#define PMD_SHIFT (21)
#define PMD_SIZE (_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE - 1))
#define PTRS_PER_PMD (512)

#define PUD_SHIFT (30)
#define PUD_SIZE (_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK (~(PUD_SIZE - 1))
#define PTRS_PER_PUD (512)

#define PGDIR_SHIFT (39)
#define PGDIR_SIZE (_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK (~(PGDIR_SIZE - 1))
#define PTRS_PER_PGD (512)

#define PGD_INDEX(va) (((va) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define PUD_INDEX(va) (((va) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define PMD_INDEX(va) (((va) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define PTE_INDEX(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

/* 页表属性类型,底层判断实现 */
#define PAGE_MEMORY (0x1)
#define PAGE_DEVICE (0x2)

#endif
