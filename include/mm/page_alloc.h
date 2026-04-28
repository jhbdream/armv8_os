#ifndef _MM_PAGE_ALLOC_H
#define _MM_PAGE_ALLOC_H

#include <mm/page.h>

struct page *alloc_pages(unsigned int order);
void free_pages(struct page *page, unsigned int order);

unsigned long page_to_phys(struct page *page);
struct page *phys_to_page(unsigned long phys);
void *page_to_virt(struct page *page);
struct page *virt_to_page(void *vaddr);

void mm_init(void);

#endif
