#ifndef _MM_PAGE_H
#define _MM_PAGE_H

#include <eeos/list.h>

#define PAGE_SHIFT 12
#define PAGE_SIZE  (1UL << PAGE_SHIFT)
#define PAGE_MASK  (~(PAGE_SIZE - 1))

#define MAX_ORDER 10

struct page {
    unsigned long flags;
    struct list_head lru;
    unsigned int order;
    unsigned int refcount;
};

#define set_page_refcount(page, n) ((page)->refcount = (n))
#define get_page(page)             ((page)->refcount++)
#define put_page(page)             (--(page)->refcount)

#endif
