#ifndef _PAGE_ALLOC_H_
#define _PAGE_ALLOC_H_

struct page
{
	unsigned long flag;
	void *free_list;
};

extern struct page *page_base;

#define __pfn_to_page(pfn) (page_base + (pfn))
#define __page_to_pfn(page) ((unsigned long)((page) - page_base))

#define pfn_to_page __pfn_to_page
#define page_to_pfn __page_to_pfn

#endif
