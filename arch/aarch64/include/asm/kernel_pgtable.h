/**
 * @file kernel_pgtable.h
 * @author jihongbin (longma@orbbec.com)
 * @brief
 * @version 0.1
 * @date 2022-05-07
 * @copyright Copyright (c) {2015-2021} Orbbec
 *
 */

#ifndef __ASM_KERNEL_PGTABLE_H
#define __ASM_KERNEL_PGTABLE_H

#include <asm/pgtable_hwdef.h>

/*
 * The linear mapping and the start of memory are both 2M aligned (per
 * the arm64 booting.txt requirements). Hence we can use section mapping
 * with 4K (section size = 2M) but not with 16K (section size = 32M) or
 * 64K (section size = 512M).
 */
#ifdef CONFIG_ARM64_4K_PAGES
#define ARM64_KERNEL_USES_PMD_MAPS 1
#else
#define ARM64_KERNEL_USES_PMD_MAPS 0
#endif

/*
 * The idmap and swapper page tables need some space reserved in the kernel
 * image. Both require pgd, pud (4 levels only) and pmd tables to (section)
 * map the kernel. With the 64K page configuration, swapper and idmap need to
 * map to pte level. The swapper also maps the FDT (see __create_page_tables
 * for more information). Note that the number of ID map translation levels
 * could be increased on the fly if system RAM is out of reach for the default
 * VA range, so pages required to map highest possible PA are reserved in all
 * cases.
 */
#if ARM64_KERNEL_USES_PMD_MAPS
#define SWAPPER_PGTABLE_LEVELS (CONFIG_PGTABLE_LEVELS - 1)
#define IDMAP_PGTABLE_LEVELS (ARM64_HW_PGTABLE_LEVELS(PHYS_MASK_SHIFT) - 1)
#else
#define SWAPPER_PGTABLE_LEVELS (CONFIG_PGTABLE_LEVELS)
#define IDMAP_PGTABLE_LEVELS (ARM64_HW_PGTABLE_LEVELS(PHYS_MASK_SHIFT))
#endif

#define EARLY_KASLR (0)

#define EARLY_ENTRIES(vstart, vend, shift)                                     \
	((((vend)-1) >> (shift)) - ((vstart) >> (shift)) + 1 + EARLY_KASLR)

#define EARLY_PGDS(vstart, vend) (EARLY_ENTRIES(vstart, vend, PGDIR_SHIFT))

#if SWAPPER_PGTABLE_LEVELS > 3
#define EARLY_PUDS(vstart, vend) (EARLY_ENTRIES(vstart, vend, PUD_SHIFT))
#else
#define EARLY_PUDS(vstart, vend) (0)
#endif

#if SWAPPER_PGTABLE_LEVELS > 2
#define EARLY_PMDS(vstart, vend)                                               \
	(EARLY_ENTRIES(vstart, vend, SWAPPER_TABLE_SHIFT))
#else
#define EARLY_PMDS(vstart, vend) (0)
#endif

#define EARLY_PAGES(vstart, vend)                                              \
	(1 /* PGDIR page */                                                    \
	 + EARLY_PGDS((vstart),                                                \
		      (vend)) /* each PGDIR needs a next level page table */   \
	 + EARLY_PUDS((vstart),                                                \
		      (vend)) /* each PUD needs a next level page table */     \
	 + EARLY_PMDS((vstart),                                                \
		      (vend))) /* each PMD needs a next level page table */
#define INIT_DIR_SIZE (PAGE_SIZE * EARLY_PAGES(KIMAGE_VADDR, __kimage_end))
#define IDMAP_DIR_SIZE (IDMAP_PGTABLE_LEVELS * PAGE_SIZE)

/* Initial memory map size */
#if ARM64_KERNEL_USES_PMD_MAPS
#define SWAPPER_BLOCK_SHIFT PMD_SHIFT
#define SWAPPER_BLOCK_SIZE PMD_SIZE
#define SWAPPER_TABLE_SHIFT PUD_SHIFT
#else
#define SWAPPER_BLOCK_SHIFT PAGE_SHIFT
#define SWAPPER_BLOCK_SIZE PAGE_SIZE
#define SWAPPER_TABLE_SHIFT PMD_SHIFT
#endif

/*
 * Initial memory map attributes.
 */
#define SWAPPER_PTE_FLAGS (PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define SWAPPER_PMD_FLAGS (PMD_TYPE_SECT | PMD_SECT_AF | PMD_SECT_S)

#if ARM64_KERNEL_USES_PMD_MAPS
#define SWAPPER_MM_MMUFLAGS (PMD_ATTRINDX(MT_NORMAL) | SWAPPER_PMD_FLAGS)
#else
#define SWAPPER_MM_MMUFLAGS (PTE_ATTRINDX(MT_NORMAL) | SWAPPER_PTE_FLAGS)
#endif

#endif