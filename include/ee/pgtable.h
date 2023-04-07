#ifndef _EE_PGTABLE_H
#define _EE_PGTABLE_H

#include <type.h>
#include <asm/pgtable.h>

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

/* 页表属性类型 */
#define PAGE_MEMORY _pgprot(PROT_NORMAL)
#define PAGE_DEVICE _pgprot(PROT_DEVICE_nGnRnE)

static inline pgd_t *pgd_offset_pgd(pgd_t *pgd, unsigned long addr)
{
	return (pgd + PGD_INDEX(addr));
}

static inline pud_t *pud_offset_pud(pud_t *pud, unsigned long addr)
{
	return (pud + PUD_INDEX(addr));
}

static inline pmd_t *pmd_offset_pmd(pmd_t *pmd, unsigned long addr)
{
	return (pmd + PMD_INDEX(addr));
}

static inline pte_t *pte_offset_pte(pte_t *pte, unsigned long addr)
{
	return (pte + PTE_INDEX(addr));
}

/* 根据虚拟地址查找内核页表 PGD 表项 */
/* swapper_pg_dir 是内核页表 */
extern pgd_t swapper_pg_dir[PTRS_PER_PGD];
static inline pgd_t *pgd_offset_k(unsigned long addr)
{
	return ((pgd_t *)swapper_pg_dir + PGD_INDEX(addr));
}

#undef __va
#define __va(x) (0)
#define pgd_present(pgd) (0)
#define pud_present(pgd) (0)
#define pmd_present(pgd) (0)
#define pte_present(pgd) (0)

static inline void pgd_populate(pgd_t *pgd, pud_t *pud, pgdval_t prot)
{

}

static inline void pud_populate(pud_t *pud, pmd_t *pmd, pudval_t prot)
{
}


static inline void pmd_populate(pmd_t *pmd, pte_t *pte, pmdval_t prot)
{
}

static inline void pte_populate(pte_t *pte, phys_addr_t page, pteval_t prot)
{
}
#define PGD_TYPE_TABLE (3)

#define pgd_page_vaddr(pgd)                                                    \
	((unsigned long)__va((unsigned long)pgd_val(pgd) & PTE_ADDR_MASK))
#define pud_page_vaddr(pud)                                                    \
	((unsigned long)__va((unsigned long)pud_val(pud) & PTE_ADDR_MASK))
#define pmd_page_vaddr(pmd)                                                    \
	((unsigned long)__va((unsigned long)pmd_val(pmd) & PTE_ADDR_MASK))

static inline pud_t *pud_offset(pgd_t *pgd, unsigned long address)
{
	return (pud_t *)pgd_page_vaddr(*(pgd)) + PUD_INDEX(address);
}

static inline pmd_t *pmd_offset(pud_t *pud, unsigned long address)
{
	return (pmd_t *)pud_page_vaddr(*(pud)) + PMD_INDEX(address);
}

static inline pte_t *pte_offset(pmd_t *pmd, unsigned long address)
{
	return (pte_t *)pmd_page_vaddr(*(pmd)) + PTE_INDEX(address);
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
#define pgd_addr_end(addr, end)                                                \
	({                                                                     \
		unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK; \
		(__boundary - 1 < (end)-1) ? __boundary : (end);               \
	})
#endif

#ifndef pud_addr_end
#define pud_addr_end(addr, end)                                                \
	({                                                                     \
		unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;     \
		(__boundary - 1 < (end)-1) ? __boundary : (end);               \
	})
#endif

#ifndef pmd_addr_end
#define pmd_addr_end(addr, end)                                                \
	({                                                                     \
		unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;     \
		(__boundary - 1 < (end)-1) ? __boundary : (end);               \
	})
#endif

#endif
