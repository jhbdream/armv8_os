#include <stdint.h>

#include <ee/pgtable.h>

#include <type.h>

#include <asm/memory.h>

#include <compiler_attribute.h>

#include <io.h>

#include "mmu_v3.h"

/*
 * Hardware page table definitions.
 *
 * Level -1 descriptor (PGD).
 */
#define PGD_TYPE_TABLE  ((pgdval_t)3 << 0)
#define PGD_TABLE_BIT   ((pgdval_t)1 << 1)
#define PGD_TYPE_MASK   ((pgdval_t)3 << 0)
#define PGD_TABLE_AF    ((pgdval_t)1 << 10) /* Ignored if no FEAT_HAFT */
#define PGD_TABLE_PXN   ((pgdval_t)1 << 59)
#define PGD_TABLE_UXN   ((pgdval_t)1 << 60)

/*
 * Level 1 descriptor (PUD).
 */
#define PUD_TYPE_TABLE  ((pud_t)3 << 0)
#define PUD_TABLE_BIT   ((pud_t)1 << 1)
#define PUD_TYPE_MASK   ((pud_t)3 << 0)
#define PUD_TYPE_SECT   ((pud_t)1 << 0)
#define PUD_SECT_RDONLY ((pud_t)1 << 7)  /* AP[2] */
#define PUD_TABLE_AF    ((pud_t)1 << 10) /* Ignored if no FEAT_HAFT */
#define PUD_TABLE_PXN   ((pud_t)1 << 59)
#define PUD_TABLE_UXN   ((pud_t)1 << 60)

/*
 * Level 2 descriptor (PMD).
 */
#define PMD_TYPE_MASK   ((pmd_t)3 << 0)
#define PMD_TYPE_TABLE  ((pmd_t)3 << 0)
#define PMD_TYPE_SECT   ((pmd_t)1 << 0)
#define PMD_TABLE_BIT   ((pmd_t)1 << 1)
#define PMD_TABLE_AF    ((pmd_t)1 << 10) /* Ignored if no FEAT_HAFT */

/*
 * Section
 */
#define PMD_SECT_USER   ((pmd_t)1 << 6) /* AP[1] */
#define PMD_SECT_RDONLY ((pmd_t)1 << 7) /* AP[2] */
#define PMD_SECT_S      ((pmd_t)3 << 8)
#define PMD_SECT_AF     ((pmd_t)1 << 10)
#define PMD_SECT_NG     ((pmd_t)1 << 11)
#define PMD_SECT_CONT   ((pmd_t)1 << 52)
#define PMD_SECT_PXN    ((pmd_t)1 << 53)
#define PMD_SECT_UXN    ((pmd_t)1 << 54)
#define PMD_TABLE_PXN   ((pmd_t)1 << 59)
#define PMD_TABLE_UXN   ((pmd_t)1 << 60)

/*
 * Level 3 descriptor (PTE).
 */
#define PTE_VALID       ((pte_t)1 << 0)
#define PTE_TYPE_MASK   ((pte_t)3 << 0)
#define PTE_TYPE_PAGE   ((pte_t)3 << 0)
#define PTE_TABLE_BIT   ((pte_t)1 << 1)
#define PTE_USER        ((pte_t)1 << 6)  /* AP[1] */
#define PTE_RDONLY      ((pte_t)1 << 7)  /* AP[2] */
#define PTE_SHARED      ((pte_t)3 << 8)  /* SH[1:0], inner shareable */
#define PTE_AF          ((pte_t)1 << 10) /* Access Flag */
#define PTE_NG          ((pte_t)1 << 11) /* nG */
#define PTE_GP          ((pte_t)1 << 50) /* BTI guarded */
#define PTE_DBM         ((pte_t)1 << 51) /* Dirty Bit Management */
#define PTE_CONT        ((pte_t)1 << 52) /* Contiguous range */
#define PTE_PXN         ((pte_t)1 << 53) /* Privileged XN */
#define PTE_UXN         ((pte_t)1 << 54) /* User XN */

uint64_t kimage_voffset = 0;

static pgd_t pgd_table[PTRS_PER_PGD] __aligned(4096);
static pud_t pud_table[PTRS_PER_PUD] __aligned(4096);
static pud_t pmd_table[PTRS_PER_PMD * 16] __aligned(4096);

void switch_mm(void)
{
	// 设置TTBR1寄存器
	asm volatile("msr ttbr1_el1, %0\n"
		     "isb" ::"r"(virt_to_phys(pgd_table)));

	// 切换后刷新本地TLB
	asm volatile("tlbi vmalle1is"); // 使所有EL1 TLB条目失效
}

/**
 * @brief 创建3级页表，内核映射线性 + 镜像映射
 */
void create_kernel_map(void)
{
	extern unsigned long __kimage_start[], __kimage_end[];

	uint64_t index;
	uint64_t va, va_start, va_end;

	pgd_t *pgdp;

	phys_addr_t phys = PHYS_OFFSET;

	phys_addr_t pa;

	pgdval_t pgd_val;

	va_start = (uint64_t)__kimage_start;
	va_end   = (uint64_t)__kimage_end;

    // init pgd table
	index = PGD_INDEX(va_start);
	pgdp  = &pgd_table[index];

	pa      = virt_to_phys(&pud_table[0]);
	pgd_val = (pa & PMD_MASK) | (PGD_TYPE_TABLE) | (PGD_TABLE_UXN) | (PGD_TABLE_AF);

	writel(pgd_val, pgdp);

	pa = virt_to_phys(__kimage_start);

	// init pgd
	//
	while (1)
		;
}
