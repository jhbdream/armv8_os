#include "errno.h"
#include "mm/memblock.h"
#include "printk.h"
#include <mm/page_alloc.h>
#include <ee/pgtable.h>
#include <round.h>
#include <asm-generic/bug.h>

/* use buddy page alloc */

struct page *page_base = NULL;

static void inline page_base_set(struct page *page)
{
	page_base = page;
}

static struct page *page_base_get(void)
{
	return page_base;
}

/*
 * page_base
 * +-------+-------+-------+-------+-------+-------+
 * | page  | page  |  ...  | ..... | page  | page  |
 * +-------+-------+-------+-------+-------+-------+
 */
int vmemmap_page_init(unsigned long mem_start, unsigned long mem_end)
{
	int ret = 0;
    unsigned long size, align;
	struct page *page;
	unsigned long page_count = DIV_ROUND_UP((mem_end - mem_start), PAGE_SIZE);
	size = sizeof(struct page) * page_count;

	printk("buddy init mem range: [0x%08x - 0x%08x]\n", mem_start, mem_end);
	printk("total page count: [x%x]\n", page_count);
	printk("total page size: [0x%x]\n", size);

	page = memblock_alloc(size, PAGE_SIZE);
	if(!page)
	{
		ret = -ENOMEM;
		printk("vmemmap page init failed %d!", ret);
		return ret;
	}

	page_base_set(page);
	printk("page base: [0x%016lx]\n", page_base_get());

	return ret;
}

int buddy_init(void)
{
	int ret = 0;

	return ret;
}

void free_pages(struct page *page, unsigned int order)
{

}

/* 将 membloc 空闲内存加入到buddy */
void memblock_free_pages(unsigned long pfn, unsigned int order)
{
	printk("[%s]: pfn 0x%x order %d\n", __func__, pfn, order);

}

