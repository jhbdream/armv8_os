#include "printk.h"
#include <mm/page_alloc.h>
#include <ee/pgtable.h>
#include <round.h>

/* use buddy page alloc */

static struct page *page_base;

int buddy_page_init(unsigned long mem_start, unsigned long mem_end)
{
    unsigned long size, align;
	unsigned long page_count = DIV_ROUND_UP((mem_end - mem_start), PAGE_SIZE);
	size = sizeof(struct page) * page_count;
	printk("total page count: [0x%x]\n", page_count);
	printk("total page size: [0x%x]\n", size);

	return 0;
}
