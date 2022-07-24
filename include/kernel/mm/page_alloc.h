#ifndef _PAGE_ALLOC_H_
#define _PAGE_ALLOC_H_

#include <type.h>
#include <asm/memory.h>

#define MAX_ORDER 11

struct free_area
{
    struct list_head    free_list;
    unsigned long       nr_free;
};

/* page 结构体分时复用，根据pagetype决定使用哪一种类型 */
struct page
{
	union
	{
		struct
		{
			struct list_head node;
			unsigned long private;
		};
	};

	unsigned int page_type;
};

struct zone
{
	struct free_area free_area[MAX_ORDER];
};

extern struct page *page_base;

#ifndef ARCH_PFN_OFFSET
#define ARCH_PFN_OFFSET     (0UL)
#endif

#define __pfn_to_page(pfn) (page_base + (pfn) - ARCH_PFN_OFFSET)
#define __page_to_pfn(page) ((unsigned long)((page) - page_base) + ARCH_PFN_OFFSET)

#define pfn_to_page __pfn_to_page
#define page_to_pfn __page_to_pfn


void memblock_free_pages(unsigned long pfn, unsigned int order);

static inline unsigned int buddy_order(struct page *page)
{
	return page->private;
}

static inline void set_page_private(struct page *page, unsigned long private)
{
    page->private = private;
}

#endif
