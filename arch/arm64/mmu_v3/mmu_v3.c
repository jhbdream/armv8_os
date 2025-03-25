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
#define PUD_TYPE_TABLE  ((pudval_t)3 << 0)
#define PUD_TABLE_BIT   ((pudval_t)1 << 1)
#define PUD_TYPE_MASK   ((pudval_t)3 << 0)
#define PUD_TYPE_SECT   ((pudval_t)1 << 0)
#define PUD_SECT_RDONLY ((pudval_t)1 << 7)  /* AP[2] */
#define PUD_TABLE_AF    ((pudval_t)1 << 10) /* Ignored if no FEAT_HAFT */
#define PUD_TABLE_PXN   ((pudval_t)1 << 59)
#define PUD_TABLE_UXN   ((pudval_t)1 << 60)

/*
 * Level 2 descriptor (PMD).
 */
#define PMD_TYPE_MASK   ((pmdval_t)3 << 0)
#define PMD_TYPE_TABLE  ((pmdval_t)3 << 0)
#define PMD_TYPE_SECT   ((pmdval_t)1 << 0)
#define PMD_TABLE_BIT   ((pmdval_t)1 << 1)
#define PMD_TABLE_AF    ((pmdval_t)1 << 10) /* Ignored if no FEAT_HAFT */

/*
 * Section
 */
#define PMD_SECT_USER   ((pmdval_t)1 << 6) /* AP[1] */
#define PMD_SECT_RDONLY ((pmdval_t)1 << 7) /* AP[2] */
#define PMD_SECT_S      ((pmdval_t)3 << 8)
#define PMD_SECT_AF     ((pmdval_t)1 << 10)
#define PMD_SECT_NG     ((pmdval_t)1 << 11)
#define PMD_SECT_CONT   ((pmdval_t)1 << 52)
#define PMD_SECT_PXN    ((pmdval_t)1 << 53)
#define PMD_SECT_UXN    ((pmdval_t)1 << 54)
#define PMD_TABLE_PXN   ((pmdval_t)1 << 59)
#define PMD_TABLE_UXN   ((pmdval_t)1 << 60)

#define PMD_ATTRINDX(t) ((pmdval_t)(t) << 2)

/*
 * Level 3 descriptor (PTE).
 */
#define PTE_VALID       ((pteval_t)1 << 0)
#define PTE_TYPE_MASK   ((pteval_t)3 << 0)
#define PTE_TYPE_PAGE   ((pteval_t)3 << 0)
#define PTE_TABLE_BIT   ((pteval_t)1 << 1)
#define PTE_USER        ((pteval_t)1 << 6)  /* AP[1] */
#define PTE_RDONLY      ((pteval_t)1 << 7)  /* AP[2] */
#define PTE_SHARED      ((pteval_t)3 << 8)  /* SH[1:0], inner shareable */
#define PTE_AF          ((pteval_t)1 << 10) /* Access Flag */
#define PTE_NG          ((pteval_t)1 << 11) /* nG */
#define PTE_GP          ((pteval_t)1 << 50) /* BTI guarded */
#define PTE_DBM         ((pteval_t)1 << 51) /* Dirty Bit Management */
#define PTE_CONT        ((pteval_t)1 << 52) /* Contiguous range */
#define PTE_PXN         ((pteval_t)1 << 53) /* Privileged XN */
#define PTE_UXN         ((pteval_t)1 << 54) /* User XN */

uint64_t kimage_voffset = 0;

extern unsigned long __kimage_start[], __kimage_end[];

static pgd_t pgd_table[PTRS_PER_PGD] __aligned(4096);
static pud_t pud_table[PTRS_PER_PUD] __aligned(4096);
static pmd_t pmd_table[PTRS_PER_PMD * 16] __aligned(4096);

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

	// init pgd

	while (1)
		;
}

/**
 * 初始化页表，将虚拟地址0xFFFF000000000000开始的连续区域映射到物理内存
 * @param phys_start 起始物理地址（需2MB对齐）
 * @param size       映射区域总大小（需2MB对齐）
 */
void init_pagetables(void)
{
	// 1. 获取页表物理地址
	uint64_t pgd_phys = virt_to_phys(pgd_table);
	uint64_t pud_phys = virt_to_phys(pud_table);
	uint64_t pmd_phys = virt_to_phys(pmd_table);

	pmdval_t pmd;

	uint64_t va, va_start, va_end;
	uint64_t phys_start, size;

	phys_addr_t pa;

	// 2. 配置PGD表项 -> PUD表
	pgd_t *pgd_entry = &pgd_table[PGD_INDEX(0xFFFF000000000000)];
	writeq(pud_phys | PMD_TYPE_TABLE, pgd_entry);

	// 3. 配置PUD表项 -> PMD表
	pud_t *pud_entry = &pud_table[PUD_INDEX(0xFFFF000000000000)];
	writeq(pmd_phys | PMD_TYPE_TABLE, pud_entry);

	// 4. 配置PMD表项为2MB大页（Block descriptor）
	va_start = (uint64_t)__kimage_start;
	va_end   = (uint64_t)__kimage_end;

	va   = va_start;
	pa   = va_start - kimage_voffset;
	size = va_end - va_start;

	uint64_t num_entries = size >> PMD_SHIFT;

	for (int i = 0; i < num_entries; i++) {
		pmd_t *pmd_entry = &pmd_table[PMD_INDEX(va)];
		pmd              = pa | PMD_TYPE_SECT // 块描述符
		      | PMD_SECT_AF                   // Access Flag
		      | PMD_SECT_PXN                  //
		      | PMD_ATTRINDX(MT_NORMAL)       // 内存类型
		      | PMD_SECT_S;                   // 可共享

		writeq(pmd, pmd_entry);

		va += (1 << PMD_SHIFT); // 增加2MB虚拟地址
		pa += (1 << PMD_SHIFT); // 增加2MB物理地址
	}

	// 5. 刷新TLB并设置TTBR1_EL1
	asm volatile("tlbi vmalle1is"); // 无效化所有TLB条目
	asm volatile("dsb sy");
	asm volatile("isb");
	asm volatile("msr ttbr1_el1, %0" ::"r"(pgd_phys)); // 设置页表基址
	asm volatile("dsb sy");
	asm volatile("isb");
}
