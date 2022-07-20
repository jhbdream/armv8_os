#include "asm/pgtable.h"
#include "stddef.h"
#include <ee/pgtable.h>
#include <ee/pfn.h>
#include <asm-generic/bug.h>
#include <string.h>
#include <type.h>
#include <linkage.h>
#include <mm/memblock.h>
#include <asm/memory.h>

extern char __kimage_start[];
extern char __kimage_end[];

/* early pgd */
pgd_t early_pg_dir[PTRS_PER_PGD] __aligned(PAGE_SIZE);
pgd_t trampoline_pg_dir[PTRS_PER_PGD] __page_aligned_bss;
static pmd_t trampoline_pmd[PTRS_PER_PMD] __page_aligned_bss;
static pmd_t early_pmd[PTRS_PER_PMD] __aligned(PAGE_SIZE);

static pmd_t fixmap_pmd[PTRS_PER_PMD] __page_aligned_bss;
static pte_t fixmap_pte[PTRS_PER_PTE] __page_aligned_bss;

struct pt_alloc_ops {
	pte_t *(*get_pte_virt)(phys_addr_t pa);
	phys_addr_t (*alloc_pte)(uintptr_t va);
	pmd_t *(*get_pmd_virt)(phys_addr_t pa);
	phys_addr_t (*alloc_pmd)(uintptr_t va);
};

static struct pt_alloc_ops _pt_ops;
#define pt_ops _pt_ops

static void create_pte_mapping(pte_t *ptep,
				      uintptr_t va, phys_addr_t pa,
				      phys_addr_t sz, pgprot_t prot)
{
	uintptr_t pte_idx = pte_index(va);

	BUG_ON(sz != PAGE_SIZE);

	if (pte_none(ptep[pte_idx]))
		ptep[pte_idx] = pfn_pte(PFN_DOWN(pa), prot);
}

static void create_pmd_mapping(pmd_t *pmdp,
				      uintptr_t va, phys_addr_t pa,
				      phys_addr_t sz, pgprot_t prot)
{
	pte_t *ptep;
	phys_addr_t pte_phys;
	uintptr_t pmd_idx = pmd_index(va);

	if (sz == PMD_SIZE) {
		if (pmd_none(pmdp[pmd_idx]))
			pmdp[pmd_idx] = pfn_pmd(PFN_DOWN(pa), prot);
		return;
	}

	if (pmd_none(pmdp[pmd_idx])) {
		pte_phys = pt_ops.alloc_pte(va);
		pmdp[pmd_idx] = pfn_pmd(PFN_DOWN(pte_phys), PAGE_TABLE);
		ptep = pt_ops.get_pte_virt(pte_phys);
		memset(ptep, 0, PAGE_SIZE);
	} else {
		pte_phys = PFN_PHYS(_pmd_pfn(pmdp[pmd_idx]));
		ptep = pt_ops.get_pte_virt(pte_phys);
	}

	create_pte_mapping(ptep, va, pa, sz, prot);
}

void create_pgd_mapping(pgd_t *pgdp,
				uintptr_t va, phys_addr_t pa,
				phys_addr_t sz, pgprot_t prot)
{
	pmd_t *pmdp;
	phys_addr_t next_phys;
	uintptr_t pgd_idx = pgd_index(va);

	// 如果创建映射大小与一级页表相同 只创建一级映射就可以
	// 创建完成后返回
	if (sz == PGDIR_SIZE) {
		if (pgd_val(pgdp[pgd_idx]) == 0)
			pgdp[pgd_idx] = pfn_pgd(PFN_DOWN(pa), prot);
		return;
	}

	if (pgd_val(pgdp[pgd_idx]) == 0) {
		next_phys = pt_ops.alloc_pmd(va);
		pgdp[pgd_idx] = pfn_pgd(PFN_DOWN(next_phys), PAGE_TABLE);
		pmdp = pt_ops.get_pmd_virt(next_phys);
		memset(pmdp, 0, PAGE_SIZE);
	} else {
		next_phys = PFN_PHYS(_pgd_pfn(pgdp[pgd_idx]));
		pmdp = pt_ops.get_pmd_virt(next_phys);
	}

	create_pmd_mapping(pmdp, va, pa, sz, prot);
}

static pmd_t *get_pmd_virt_early(phys_addr_t pa)
{
	/* Before MMU is enabled */
	return (pmd_t *)((uintptr_t)pa);
}

static phys_addr_t alloc_pmd_early(uintptr_t va)
{
	return (uintptr_t)early_pmd;
}

void setup_vm(void)
{
	pt_ops.alloc_pte = NULL;
	pt_ops.get_pte_virt = NULL;
	pt_ops.alloc_pmd = alloc_pmd_early;
	pt_ops.get_pmd_virt = get_pmd_virt_early;

#if 0
	/* create fixmap map
	 * va = FIXADDR_START
	 * pa = fixmap_pmd
	 * size = 1G
	 *
	 * 因为 fixmap 的页表是定义成了全局变量
	 * 所以在建立了镜像的代码段和数据段之后
	 * 就可以直接去访问全局变量fixmap_pte 来修改 fixmap 的页表映射关系
	 * 可以方便的去做一些临时的 4K 大小的映射
	 *
	 */
	create_pgd_mapping(early_pg_dir, FIXADDR_START,
			   (uintptr_t)fixmap_pmd, PGDIR_SIZE, PAGE_TABLE);

	/* Setup fixmap PMD */
	create_pmd_mapping(fixmap_pmd, FIXADDR_START,
			   (uintptr_t)fixmap_pte, PMD_SIZE, PAGE_TABLE);
#endif
	/**
	 * create temp kernel image map
	 * va = KIMAGE_VADDR
	 * pa = load_addr
	 * size = 2M ? PMD_SIZE
	 */
	create_pgd_mapping(trampoline_pg_dir, KIMAGE_VADDR,
			   (uintptr_t)trampoline_pmd, PGDIR_SIZE, PAGE_TABLE);
	create_pmd_mapping(trampoline_pmd, KIMAGE_VADDR,
			  (uintptr_t)&__kimage_start, PMD_SIZE, PAGE_KERNEL_EXEC);
#if 0
	/**
	 * create kernel image map in early_pd_dir
	 *
	 */
	phys_addr_t kernel_image_start = (phys_addr_t)&__kimage_start;
	phys_addr_t kernel_image_end = (phys_addr_t)&__kimage_end;
	phys_addr_t kernel_image_size = kernel_image_end - kernel_image_start;

	uintptr_t pa = kernel_image_start;
	uintptr_t va = KIMAGE_VADDR;
	uintptr_t va_end = va + kernel_image_size;

	for(; va < va_end; va += PMD_SIZE)
	{
		/* pmd = early_pmd */
		/* size = 1g is enough */
		create_pgd_mapping(early_pg_dir, va, pa + (va - KIMAGE_VADDR), PMD_SIZE, PAGE_KERNEL_EXEC);
	}
#endif
}

