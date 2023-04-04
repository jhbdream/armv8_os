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

#define PAGE_SHIFT (12)
#define PAGE_SIZE (_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PTRS_PER_PTE (512)

/*
 *
 * PMD_SHIFT determines the size a level 2 page table entry can map.
 */
#define PMD_SHIFT (21)
#define PMD_SIZE (_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE - 1))
#define PTRS_PER_PMD (512)

/*
 * PUD_SHIFT determines the size a level 1 page table entry can map.
 */
#define PUD_SHIFT (30)
#define PUD_SIZE (_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK (~(PUD_SIZE - 1))
#define PTRS_PER_PUD (512)

/*
 * PGDIR_SHIFT determines the size a top-level page table entry can map
 * (depending on the configuration, this level can be 0, 1 or 2).
 */
#define PGDIR_SHIFT (39)
#define PGDIR_SIZE (_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK (~(PGDIR_SIZE - 1))
#define PTRS_PER_PGD (512)

#define PGD_INDEX(va) (((va) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define PUD_INDEX(va) (((va) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define PMD_INDEX(va) (((va) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define PTE_INDEX(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

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
	BUG_ON(0 == pgprot_val(prot));
	BUG_ON(pgtable_alloc == NULL);

	unsigned long addr, end, next;
	pgd_t *pgdp;
	pgdp = pgdir + PGD_INDEX(virt);

	phys = phys & PAGE_MASK;
	addr = virt & PAGE_MASK;
	end = addr + size;

	do {
		next = addr + PGDIR_SIZE;
		if (end < next)
			end = next;

	} while (pgdp++, addr = next, addr != end);
}
