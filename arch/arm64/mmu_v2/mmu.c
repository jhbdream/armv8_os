#include "asm-generic/bug.h"
#include "asm/memory.h"

#include "pgtable.h"
#include "const.h"
#include "stddef.h"
#include "mmu.h"

/**
 * 1. 考虑与 vmap 共用接口
 * 2. 实现 fix_map 机制, 用于 page_alloc 实现之前使用
 * 3. 实现恒等映射，定义静态恒等映射页表
 * 4. 使用统一的页表分配接口时，如何在内存分配接口生效之前分配内存
 * 5. 早期虚拟地址物理地址转换接口？
 * 6. 中期虚拟地址物理地址转换接口?
 */

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
static void create_pte_mapping(pte_t *ptep, unsigned long addr, unsigned long end,
			       unsigned long phys, pgprot_t prot)
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

	} while (ptep++);
}

static void create_pmd_mapping(pmd_t *pmdp, unsigned long addr, unsigned long end,
			       unsigned long phys, pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int),
			       unsigned long (*pgtable_get_va)(unsigned long))
{
	BUG_ON(pmdp == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pmd_t        *pmd;
	pte_t        *ptep;
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
			set_pmd(pmdp, _pmd(pte_page | PAGE_TYPE_TABLE));
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

static void create_pud_mapping(pud_t *pudp, unsigned long addr, unsigned long end,
			       unsigned long phys, pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int),
			       unsigned long (*pgtable_get_va)(unsigned long))
{
	BUG_ON(pudp == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pud_t        *pud;
	pmd_t        *pmdp;
	unsigned long next;
	unsigned long pmd_page;

	pud = pudp + PMD_INDEX(addr);

	do {
		pmd_page = pud_val(*pud);
		if (!pmd_page) {
			// 下一级页表为空，需要分配一个页表并填充到pudp中
			// 返回的是物理地址
			pmd_page = pgtable_alloc(PMD_PAGE);

			// 填充下一级页表物理地址到当前表项
			// 同时需要补充其他位域的内容
			set_pud(pudp, _pud(pmd_page | PAGE_TYPE_TABLE));
		}

		// 将pmd的物理地址转换为虚拟地址
		pmdp = (pmd_t *)pgtable_get_va(pmd_page);

		/* 单次映射不能超过一个PMD大小 */
		next = (addr + PUD_SIZE) & PUD_MASK;
		if (next > end)
			next = end;

		create_pmd_mapping(pmdp, addr, next, phys, prot, pgtable_alloc, pgtable_get_va);

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
static void create_pgd_mapping(pgd_t *pgdp, unsigned long phys, unsigned long virt,
			       unsigned long size, pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int),
			       unsigned long (*pgtable_get_va)(unsigned long))
{
	BUG_ON(pgdp == NULL);
	BUG_ON(pgprot_val(prot) == 0);

	pgd_t        *pgd;
	pud_t        *pudp;
	unsigned long next;
	unsigned long pud_page;

	phys               = phys & PAGE_MASK;
	unsigned long addr = virt & PAGE_MASK;
	unsigned int  end  = addr + size;

	pgd = pgdp + PMD_INDEX(addr);

	do {
		pud_page = pgd_val(*pgd);
		if (!pud_page) {
			// 下一级页表为空，需要分配一个页表并填充到pgdp中
			// 返回的是物理地址
			pud_page = pgtable_alloc(PUD_PAGE);

			// 填充下一级页表物理地址到当前表项
			// 同时需要补充其他位域的内容

			set_pgd(pgdp, _pgd(pud_page | PAGE_TYPE_TABLE));
		}

		// 将pgd的物理地址转换为虚拟地址
		pudp = (pud_t *)pgtable_get_va(pud_page);

		/* 单次映射不能超过一个PMD大小 */
		next = (addr + PUD_SIZE) & PUD_MASK;
		if (next > end)
			next = end;

		create_pud_mapping(pudp, addr, next, phys, prot, pgtable_alloc, pgtable_get_va);

		phys += next - addr;

		addr = next;
		if (addr >= end)
			break;

	} while (pgdp++);
}

/* 简单处理 创建fixmap映射 */
void fixmap_init(pgd_t *pgdir)
{
	unsigned long va = FIXADDR_TOP;
	unsigned long pa_page;
	pgd_t        *pgdp;
	pud_t        *pudp;
	pmd_t        *pmdp;
	pte_t        *ptep;

	pgdp = pgdir + PGD_INDEX(va);
	if (pgd_val(*pgdp) == 0) {
		pa_page = (unsigned long)fixmap_pud;
		*pgdp = (fixmap_pud | 0x3);
		pudp = fixmap_pud;
	} else {
		pudp = pgd_val(*pgdp);
	}
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
	 */

	/**
	 * create kernel image map in early_pd_dir
	 *
	 */
	phys_addr_t   pa;
	unsigned long va;
	unsigned long size;

	extern unsigned long __kimage_start[], __kimage_end[];

	/* map kernel image */
	pa   = (unsigned long)__kimage_start;
	va   = (unsigned long)KIMAGE_VADDR;
	size = (unsigned long)__kimage_end - (unsigned long)__kimage_start;

	create_pgd_mapping(init_pgd, pa, va, size, _pgprot(PROT_NORMAL), early_page_alloc,
			   early_get_va);

	/* map fixmap */

	/**
	 * 1. 将 FIXMAP 虚拟地址映射到fixmap_pte页表项
	 * 2. fixmap_pte 是通过代码分配，映射了kernel后可以直接访问
	 * 3. 通过向fixmap_pte填充物理地址即可创建临时映射关系
	 * 4. 在创建线性映射，支持虚拟内存分配之前，只能分配物理内存，使用fixmap映射到虚拟内存
	 *
	 *  VA:
	 *  +------------------------------------------+
	 *  | 9 |      9 |      9 |      9 |        12 |
	 *  +------------------------------------------+
	 *
	 *  +---+
	 *  |   |
	 *  |   |
	 *  |   |
	 *  +---+
	 *  |fix|----+---+
	 *  |pgd|    |   |
	 *  +---+    |   |
	 *  |   |    |   |
	 *  |   |    +---+
	 *  |   |    |fix|----+---+
	 *  |   |    |pud|    |   |
	 *  |   |    +---+    |   |
	 *  |   |    |   |    |   |
	 *  +---+    |   |    +---+
	 *           |   |    |fix|----+---+
	 *           |   |    |pmd|    |   |
	 *           |   |    +---+    |   |
	 *           |   |    |   |    |   |
	 *           +---+    |   |    +---+
	 *                    |   |    |fix|
	 *                    |   |    |pte|
	 *                    |   |    +---+
	 *                    |   |    |   |
	 *                    +---+    |   |
	 *                             |   |
	 *                             |   |
	 *                             |   |
	 *                             |   |
	 *                             +---+
	 *
	 */
}
