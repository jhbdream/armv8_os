#include "aarch64_mmu.h"
#include "page_table.h"

#include <const.h>
#include <asm-generic/bug.h>

#include <stddef.h>

/**
 * 1. 考虑与 vmap 共用接口
 * 2. 实现 fix_map 机制, 用于 page_alloc 实现之前使用
 * 3. 实现恒等映射，定义静态恒等映射页表
 *
 */

/* config for 48bit va + 4 level page table */
#define PAGE_SHIFT (12)
#define PAGE_SIZE (_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))
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

static inline void set_pte(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
}

static void create_pte_mapping(pte_t *pte, unsigned long phys,
			       unsigned long virt, unsigned long size,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int))
{
	BUG_ON(pte == NULL);
	BUG_ON(pgtable_alloc == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pte_t *ptep;
	unsigned long addr, end, next;

	ptep = pte + PTE_INDEX(virt);

	phys = phys & PAGE_MASK;
	addr = virt & PAGE_MASK;
	end = addr + size;

	do {
		next = addr + PAGE_SIZE;
		if (next > end)
			next = end;

		// 填充PTE页表项

		set_pte(ptep, _pte(addr | pgprot_val(prot)));

	} while (ptep++, addr = next, addr != end);
}

static void create_pmd_mapping(pmd_t *pmd, unsigned long phys,
			       unsigned long virt, unsigned long size,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int))
{
}

static void create_pud_mapping(pud_t *pud, unsigned long phys,
			       unsigned long virt, unsigned long size,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int))
{
}

/**
 * @brief
 * 根据物理地址和虚拟地址创建页表映射
 *
 * @param pgdir pgd 页表基地址
 * @param phys 映射物理地址
 * @param virt 映射虚拟地址
 * @param size 映射地址空间大小
 * @param prot 映射页表项的属性
 * @param pgtable_alloc 为页表申请分配内存接口
 *
 * 1. 根据虚拟地址找到 pud 表项
 *
 */
static void create_pgd_mapping(pgd_t *pgdir, unsigned long phys,
			       unsigned long virt, unsigned long size,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int))
{
	BUG_ON(pgdir == NULL);
	BUG_ON(pgtable_alloc == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pgd_t *pgdp;
	unsigned long addr, end, next;

	pgdp = pgdir + PGD_INDEX(virt);

	phys = phys & PAGE_MASK;
	addr = virt & PAGE_MASK;
	end = addr + size;

	do {
		next = addr + PGDIR_SIZE;
		if (next > end)
			next = end;

	} while (pgdp++, addr = next, addr != end);
}
