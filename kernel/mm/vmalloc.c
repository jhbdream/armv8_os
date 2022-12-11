#include "asm/page-def.h"
#include "asm/pgtable_type.h"
#include "errno.h"
#include "mm.h"
#include "mm/memblock.h"
#include "mm/slab.h"
#include <mm/page_alloc.h>
#include "pgtable.h"
#include "printk.h"
#include <type.h>
#include <eeos.h>
#include <stddef.h>
#include <asm-generic/bug.h>
#include <ee/align.h>

struct vm_struct {
	void *addr;
	unsigned long size;
	unsigned long flags;
	struct page **page;
	unsigned int nr_pages;
	unsigned long phys_addr;
	struct vm_struct *next;
};

struct vm_struct *vmlist = NULL;

/**
 * @brief 从vmalloc地址范围区间内分配一片空闲内存
 *
 * @Param size 分配内存空间大小
 * @Param flags
 * @Param start 分配范围起始地址
 * @Param end 分配范围结束地址
 *
 * @return
 */
struct vm_struct *__get_vm_area(unsigned long size, unsigned long flags,
		unsigned long start, unsigned long end)
{
	struct vm_struct **p, *tmp, *area;
	unsigned long align = 1;
	unsigned long addr;

	/**
	 * 分配大小按照 page_size 对齐
	 */
	addr = ALIGN(start, align);
	size = ALIGN(size, PAGE_SIZE);
	if(!size)
		return NULL;

	/**
	 * 分配一个链表节点
	 */
	area = kmalloc(sizeof(*area));
	if(!area)
		return NULL;

	/**
	 *
	 * lock: vmlist 是全局变量,如果对全局变量有写相关修改 应该进行加锁操作
	 * 遍历记录的 vmstruct 信息，找到空闲的区域
	 */
	for(p = &vmlist; (tmp = *p) != NULL; p = &tmp->next)
	{
		/* 更新起始地址 */
		if((unsigned long)tmp->addr < addr)
		{
			if((unsigned long)tmp->addr + tmp->size >= addr)
				addr = ALIGN(tmp->size + (unsigned long)tmp->addr, align);

			continue;
		}

		/* addr 会变化，避免溢出 */
		if(size + addr < addr)
			goto out;

		/**
		 * 找到一片足够大小的空闲区域
		 *
		 * 找到并退出有俩种条件：
		 * 1. 链表找到末尾(要判断到末尾时剩余空间是否足够)
		 * 2. 在中间找到空隙
		 */
		if(size + addr <= (unsigned long)tmp->addr)
			goto found;

		/* 更新查找起始地址 并判断剩余空间是否足够 */
		addr = ALIGN(tmp->size + (unsigned long)tmp->addr, align);
		if(addr > end - size)
			goto out;
	}

found:

	/**
	 * 找到空闲地址范围,创建并初始化一个 vmstruct 节点,插入到链表中
	 */
	area->next = *p;
	*p = area;

	area->flags = flags;
	area->addr = (void *)addr;
	area->size = size;
	area->page = NULL;
	area->nr_pages = 0;
	area->phys_addr = 0;

	/* unlock */
	return area;
out:
	kfree(area);
	/* unlock */
	return NULL;
}

static inline int vmap_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end,
		pgprot_t prot, struct page ***page)
{
	pte_t *pte, *ptep;
	unsigned long next;

	BUG_ON(addr >= end);

	pte = pte_alloc(pmd, addr);
	if(!pte)
		return -ENOMEM;

	do
	{
		struct page *p = **page;
		if(!page)
			return -ENOMEM;

		// set_pte 把 page 的物理地址写入到 pte 表项
		ptep = pte_offset_pte(pte, addr);
		*ptep =	pfn_pte(page_to_pfn(p), prot);


		(*page)++;
	}while(pte++, addr += PAGE_SIZE, addr != end);

	return 0;
}

static inline int vmap_pmd_range(pud_t *pud, unsigned long addr, unsigned long end,
		pgprot_t prot, struct page ***page)
{
	pmd_t *pmd;
	unsigned long next;

	BUG_ON(addr >= end);

	pmd = pmd_alloc(pud, addr);
	if(!pmd)
		return -ENOMEM;

	do
	{
		next = pmd_addr_end(addr, end);
		vmap_pte_range(pmd, addr, next, prot, page);
	} while(pmd++, addr = next, addr != end);

	return 0;
}

/**
 * @brief
 *
 * @param pgd
 * @param addr 虚拟地址起始地址
 * @param end 虚拟地址结束地址
 * @param prot
 * @param page
 *
 * @return
 */
static inline int vmap_pud_range(pgd_t *pgd, unsigned long addr, unsigned long end,
		pgprot_t prot, struct page ***page)
{
	pud_t *pud;
	unsigned long next;

	BUG_ON(addr >= end);

	/* 查找对应的地址是否已经分配了表项并填充到了 PGD */
	/* 如果已经分配过 直接取出 */
	/* 如果pgd中对应pud为空 分配一个内存作为pud页表 并填充到pgd中 */

	pud = pud_alloc(pgd, addr);
	if(!pud)
		return -ENOMEM;

	do
	{
		next = pud_addr_end(addr, end);
		vmap_pmd_range(pud, addr, next, prot, page);
	} while(pud++, addr = next, addr != end);

	return 0;
}

int map_vm_area(struct vm_struct *area, pgprot_t prot, struct page ***page)
{
	pgd_t *pgd;
	unsigned long next;
	unsigned long addr = (unsigned long)area->addr;
	unsigned long end = addr + area->size;
	int err;

	BUG_ON(addr >= end);

	/* 根据地址找到pgd表项 四级页表模式下 pgd表项每一项指示 512G 空间 */
	/* 根据地址找到对于entry */
	/* PGD 表示的范围很大，不需要分配表项 一个表足够 */
	pgd = pgd_offset_k(addr);
	do
	{
		/* 如果end小于pgd的大小 next 即为end， 否则 end 为下一个pgd项地址 */
		next = pgd_addr_end(addr, end);
		vmap_pud_range(pgd, addr, next, prot, page);
	} while(pgd++, addr = next, addr != end);


	return 0;
}

/**
 *  vmap  -  map an array of pages into virtually contiguous space
 *  @pages:     array of page pointers
 *  @count:     number of pages to map
 *  @flags:     vm_area->flags
 *  @prot:      page protection for the mapping
 *
 *  Maps @count pages from @pages into contiguous kernel virtual
 *  space.
 */
void *vmap(struct page **pages, unsigned int count,
        unsigned long flags, pgprot_t prot)
{
    struct vm_struct *area;

	area = __get_vm_area(4096, 0, 0x20000000, 0x40000000);
    if (!area)
	{
		printk("get vm area failed\n");
        return NULL;
	}

    if (map_vm_area(area, prot, &pages)) {
		printk("vmap failed!\n");
        return NULL;
    }

    return area->addr;
}

int vmalloc_test(void)
{
	struct vm_struct *vm;
	void *v = NULL;
	pgprot_t prot = {0};

	printk("do vmalloc_test!\n");
	struct page *pages = __get_free_page();
	v = vmap(&pages, 1, 0, prot);
	if(v)
		printk("map va 0x%08x\n", v);
	printk("do vmalloc_end!\n");

#if 0
	printk("do vmalloc_test!\n");
	for(int i = 1; i < 32; i++)
	{
		vm = __get_vm_area(i * 4096, 0, 0, 0x40000000);
		if(!vm)
		{
			printk("__get_vm_area failed!\n");
			return -1;
		}
		printk("index[%d]: vm addr [0x%08x] size: 0x%04x \n", i, vm->addr, i * 4096);
	}
#endif

	return 0;
}

