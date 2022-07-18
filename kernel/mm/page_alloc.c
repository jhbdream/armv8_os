#include <mm/page_alloc.h>
#include <ee/pgtable.h>

/* use buddy page alloc */

static struct page *page_base;

int buddy_page_init(unsigned long mem_start, unsigned long mem_end)
{
    unsigned long size, align;
	unsigned long page_count = (mem_end - mem_start) >> PAGE_SHIFT;

	//size = sizeof(struct page) * 
	return 0;
}
