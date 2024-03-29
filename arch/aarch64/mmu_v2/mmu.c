#include "aarch64_mmu.h"
#include "page_table.h"

#include <const.h>
#include <asm-generic/bug.h>

#include <stddef.h>

/**
 * 1. 考虑与 vmap 共用接口
 * 2. 实现 fix_map 机制, 用于 page_alloc 实现之前使用
 * 3. 实现恒等映射，定义静态恒等映射页表
 * 4. 使用统一的页表分配接口时，如何在内存分配接口生效之前分配内存
 * 5. 早期虚拟地址物理地址转换接口？
 * 6. 中期虚拟地址物理地址转换接口?
 */

/* 公共宏定义 */
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

#define PTE_ADDR_MASK                                                          \
	(((_AT(pteval_t, 1) << (48 - PAGE_SHIFT)) - 1) << PAGE_SHIFT)
#define PMD_ADDR_MASK (PTE_ADDR_MASK)
#define PUD_ADDR_MASK (PTE_ADDR_MASK)
#define PGD_ADDR_MASK (PTE_ADDR_MASK)

#define PGD_INDEX(va) (((va) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define PUD_INDEX(va) (((va) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define PMD_INDEX(va) (((va) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define PTE_INDEX(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

/* 页表属性类型 */
#define PAGE_MEMORY _pgprot(PROT_NORMAL)
#define PAGE_DEVICE _pgprot(PROT_DEVICE_nGnRnE)

/* arm64 mmu 页表硬件定义 */
#define PTE_TYPE_PAGE (_AT(pteval_t, 3) << 0)
#define PTE_AF (_AT(pteval_t, 1) << 10) /* Access Flag */
#define PTE_SHARED (_AT(pteval_t, 3) << 8) /* SH[1:0], inner shareable */
#define PTE_PXN (_AT(pteval_t, 1) << 53) /* Privileged XN */
#define PTE_UXN (_AT(pteval_t, 1) << 54) /* User XN */
#define PTE_WRITE (_AT(pteval_t, 1) << 51) /* same as DBM (51) */

#define MT_NORMAL 0
#define MT_NORMAL_TAGGED 1
#define MT_NORMAL_NC 2
#define MT_DEVICE_nGnRnE 3
#define MT_DEVICE_nGnRE 4

#define PTE_ATTRINDX(t) (_AT(pteval_t, (t)) << 2)

#define PROT_DEFAULT (PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)

#define PROT_NORMAL                                                            \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL))

#define PROT_DEVICE_nGnRnE                                                     \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE |                        \
	 PTE_ATTRINDX(MT_DEVICE_nGnRnE))

// 静态定义 pgd 页表空间
// 1. 创建内核代码映射
// 2. 创建fixmap临时访问映射，用于访问内核代码之外物理空间
// 3. 如果映射到四级，只能映射2M空间，故建议只映射三级，最小映射单位是2M，映射范围是1G

pgd_t init_pgd[PTRS_PER_PGD];
pud_t init_pud[PTRS_PER_PUD];
pmd_t init_pmd[PTRS_PER_PMD];
pmd_t init_pte[PTRS_PER_PTE];

pud_t fixmap_pud[PTRS_PER_PUD];
pmd_t fixmap_pmd[PTRS_PER_PMD];
pmd_t fixmap_pte[PTRS_PER_PTE];

static inline void set_pte(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
}

static inline void set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	*pmdp = pmd;
}

static inline void set_pud(pud_t *pudp, pud_t pud)
{
	*pudp = pud;
}

static inline void set_pgd(pgd_t *pgdp, pgd_t pgd)
{
	*pgdp = pgd;
}

/**
 *  早期申请页表内存时使用
 */
typedef enum {
	NORMAL_PAGE = 0,
	PGD_PAGE,
	PUD_PAGE,
	PMD_PAGE,
	PTE_PAGE,
} pgtable_alloc_type_e;

/**
 * @brief 用于早期分配页表，分配静态定义的页表
 *
 * @param type
 *
 * @return 分配页表的物理地址
 */
static unsigned long early_page_alloc(int type)
{
	switch (type) {
	case PGD_PAGE:
		return (unsigned long)init_pgd;
	case PUD_PAGE:
		return (unsigned long)init_pud;
	case PMD_PAGE:
		return (unsigned long)init_pmd;
	case PTE_PAGE:
		return (unsigned long)init_pte;
	default:
		return (unsigned long)NULL;
	}
}

unsigned long memblock_pgtable_alloc(int type)
{
	return (unsigned long)NULL;
}

unsigned long early_get_va(unsigned long pa)
{
	return pa;
}

/**
 * 填充pte表项
 *
 * 1. size PMD表大小范围 2M
 * 2. virt + size 不能跨越PMD边界 2M
 * 3. 否则连续写pte会溢出
 * xxxx pte 是物理地址，在使用之前需要借助fixmap机制转换，使用完成之后释放
 *
 * ptep 使用虚拟地址
 * 表示pte页表指针
 */
static void create_pte_mapping(pte_t *ptep, unsigned long addr,
			       unsigned long end, unsigned long phys,
			       pgprot_t prot)
{
	BUG_ON(ptep == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pte_t *pte;
	pte = ptep + PTE_INDEX(addr);

	do {
		// 填充PTE页表项
		set_pte(pte, _pte((phys | pgprot_val(prot))));

		phys = phys + PAGE_SIZE;
		addr = addr + PAGE_SIZE;
		if (addr >= end)
			break;

	} while (pte++);
}

static void create_pmd_mapping(pmd_t *pmdp, unsigned long addr,
			       unsigned long end, unsigned long phys,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int),
			       unsigned long (*pgtable_get_va)(unsigned long))
{
	BUG_ON(pmdp == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pmd_t *pmd;
	pte_t *ptep;
	unsigned long next;
	unsigned long pte_page;

	pmd = pmdp + PMD_INDEX(addr);

	do {
		pte_page = pmd_val(*pmd);
		if (!pte_page) {
			// 下一级页表为空，需要分配一个页表并填充到pmdp中
			// 返回的是物理地址
			pte_page = pgtable_alloc(PTE_PAGE);

			// 填充下一级页表物理地址到当前表项
			// 同时需要补充其他位域的内容
			set_pmd(pmdp, _pmd(pte_page | 3));
		}

		// 将pte的物理地址转换为虚拟地址
		ptep = (pte_t *)pgtable_get_va(pte_page);

		/* 单次映射不能超过一个PMD大小 */
		next = (addr + PMD_SIZE) & PMD_MASK;
		if (next > end)
			next = end;

		create_pte_mapping(ptep, addr, next, phys, prot);

		phys += next - addr;

		addr = next;
		if (addr >= end)
			break;

	} while (pmdp++);
}

static void create_pud_mapping(pud_t *pudp, unsigned long addr,
			       unsigned long end, unsigned long phys,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int),
			       unsigned long (*pgtable_get_va)(unsigned long))
{
	BUG_ON(pudp == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pud_t *pud;
	pmd_t *pmdp;
	unsigned long next;
	unsigned long pmd_page;

	pud = pudp + PUD_INDEX(addr);

	do {
		pmd_page = pud_val(*pud);
		if (!pmd_page) {
			// 下一级页表为空，需要分配一个页表并填充到pudp中
			// 返回的是物理地址
			pmd_page = pgtable_alloc(PMD_PAGE);

			// 填充下一级页表物理地址到当前表项
			// 同时需要补充其他位域的内容
			set_pud(pudp, _pud(pmd_page | 3));
		}

		// 将pmd的物理地址转换为虚拟地址
		pmdp = (pmd_t *)pgtable_get_va(pmd_page);

		/* 单次映射不能超过一个PMD大小 */
		next = (addr + PUD_SIZE) & PUD_MASK;
		if (next > end)
			next = end;

		create_pmd_mapping(pmdp, addr, next, phys, prot, pgtable_alloc,
				   pgtable_get_va);

		phys += next - addr;

		addr = next;
		if (addr >= end)
			break;

	} while (pudp++);
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
static void create_pgd_mapping(pgd_t *pgdp, unsigned long phys,
			       unsigned long virt, unsigned long size,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int),
			       unsigned long (*pgtable_get_va)(unsigned long))
{
	BUG_ON(pgdp == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pgd_t *pgd;
	pud_t *pudp;
	unsigned long next;
	unsigned long pud_page;

	phys = phys & PAGE_MASK;
	unsigned long addr = virt & PAGE_MASK;
	unsigned long end = addr + size;

	pgd = pgdp + PGD_INDEX(addr);

	do {
		pud_page = pgd_val(*pgd);
		if (!pud_page) {
			// 下一级页表为空，需要分配一个页表并填充到pgdp中
			// 返回的是物理地址
			pud_page = pgtable_alloc(PUD_PAGE);

			// 填充下一级页表物理地址到当前表项
			// 同时需要补充其他位域的内容

			set_pgd(pgdp, _pgd(pud_page | 3));
		}

		// 将pgd的物理地址转换为虚拟地址
		pudp = (pud_t *)pgtable_get_va(pud_page);

		/* 单次映射不能超过一个PMD大小 */
		next = (addr + PUD_SIZE) & PUD_MASK;
		if (next > end)
			next = end;

		create_pud_mapping(pudp, addr, next, phys, prot, pgtable_alloc,
				   pgtable_get_va);

		phys += next - addr;

		addr = next;
		if (addr >= end)
			break;

	} while (pgdp++);
}

void create_kernel_map(void)
{
	/**
     *  1. kernel va
	 *  2. kernel pa
	 *  3. kernel size (align to 2M)
	 *  4. kernel prot
	 *
	 *  - map 3 level page_table
	 *  - only use init page_table
	 *  - arm64 qemu pa = 0x40080000
	 *
     */

	/**
	 * create kernel image map in early_pd_dir
	 *
	 */
	phys_addr_t pa;
	unsigned long va;
	unsigned long size;

	extern unsigned long __kimage_start[], __kimage_end[];
	pa = (unsigned long)__kimage_start;
	// va offset same with pa offset
	// link address
	va = 0xFFFF000000080000;
	size = (unsigned long)__kimage_end - (unsigned long)__kimage_start;

	create_pgd_mapping(init_pgd, pa, va, size, PAGE_MEMORY,
			   early_page_alloc, early_get_va);
}

void fixmap_page_init(pgd_t *pgdir)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	unsigned long fixmap_virt = 0xFFFF000010000000;
	unsigned long entry;

	pgd = pgdir + PGD_INDEX(fixmap_virt);
	if (pgd_val(*pgd) == 0) {
		entry = (unsigned long)fixmap_pud | 3;
		set_pgd(pgd, _pgd(entry));
		pud = (pud_t *)fixmap_pud;
	} else {
		entry = pgd_val(*pgd) & PGD_ADDR_MASK;
		pud = (pud_t *)__phys_to_kimg(entry);
	}

	pud = pud + PUD_INDEX(fixmap_virt);
	if (pud_val(*pud) == 0) {
		entry = (unsigned long)fixmap_pmd | 3;
		set_pud(pud, _pud(entry));
		pmd = (pmd_t *)fixmap_pmd;
	} else {
		entry = pud_val(*pud) & PUD_ADDR_MASK;
		pmd = (pmd_t *)__phys_to_kimg(entry);
	}

	pmd = pmd + PMD_INDEX(fixmap_virt);
	if (pmd_val(*pmd) == 0) {
		entry = (unsigned long)fixmap_pte | 3;
		set_pmd(pmd, _pmd(entry));
	} else {
		BUG_ON(1);
	}
}

