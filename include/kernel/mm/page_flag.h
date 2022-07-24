#ifndef __PAGE_FLAG_H_
#define __PAGE_FLAG_H_

#include <type.h>
#include <mm/page_alloc.h>

#define PAGE_TYPE_BASE  0xf0000000
#define PG_buddy    0x00000080
#define PG_table    0x00000200

#define PageType(page, flag)                        \
    ((page->page_type & (PAGE_TYPE_BASE | flag)) == PAGE_TYPE_BASE)

#define PAGE_TYPE_OPS(uname, lname)                 \
static __always_inline int Page##uname(struct page *page)       \
{                                   \
    return PageType(page, PG_##lname);              \
}                                   \
static __always_inline void __SetPage##uname(struct page *page)     \
{                                   \
    page->page_type &= ~PG_##lname;                 \
}                                   \
static __always_inline void __ClearPage##uname(struct page *page)   \
{                                   \
    page->page_type |= PG_##lname;                  \
}

PAGE_TYPE_OPS(Buddy, buddy)
PAGE_TYPE_OPS(Table, table)

#endif
