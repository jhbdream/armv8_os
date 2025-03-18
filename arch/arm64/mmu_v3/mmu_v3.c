#include <stdint.h>

#include <ee/pgtable.h>

#include <type.h>

#include <asm/memory.h>

#include <compiler_attribute.h>

#include "mmu_v3.h"

uint64_t kimage_voffset = 0;

static pgd_t pgd_table[PTRS_PER_PGD] __aligned(4096);
static pud_t pud_table[PTRS_PER_PUD] __aligned(4096);
static pud_t pmd_table[PTRS_PER_PMD * 16] __aligned(4096);

#if 0
void map_kernel(pgd_t *pgdp)
{
	unsigned long kimg_start = KIMAGE_VADDR;
	unsigned long kimg_end   = kimg_start + _end - _text;

	// 逐级创建页表项
	for (unsigned long vaddr = kimg_start; vaddr < kimg_end; vaddr += PMD_SIZE) {
		pud_t        *pud   = pud_offset(pgdp, vaddr);
		pmd_t        *pmd   = pmd_offset(pud, vaddr);
		unsigned long paddr = vaddr - kimage_voffset;

		// 设置PMD块映射（2MB粒度）
		*pmd = __pmd((paddr & PMD_MASK) | PMD_TYPE_SECT | PMD_SECT_AP_WRITE |
			     PMD_SECT_AP_READ | PMD_SECT_AF | PMD_ATTRINDX(MT_NORMAL));
	}
}

void map_phys_memory(pgd_t *pgdp)
{
	// 遍历4GB物理地址空间
	for (unsigned long paddr = PHYS_OFFSET; paddr < (PHYS_OFFSET + SZ_4G); paddr += PMD_SIZE) {
		unsigned long vaddr = paddr + LINEAR_OFFSET;
		pud_t        *pud   = pud_offset(pgdp, vaddr);
		pmd_t        *pmd   = pmd_offset(pud, vaddr);

		// 创建PMD级块映射
		*pmd = __pmd((paddr & PMD_MASK) | PMD_TYPE_SECT | PMD_SECT_AP_WRITE |
			     PMD_SECT_AP_READ | PMD_SECT_AF | PMD_ATTRINDX(MT_NORMAL));
	}
}
#endif

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

	while (1)
		;
}
