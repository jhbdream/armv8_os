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

#define __virt_to_page(x) ({ \
		unsigned long __idx = (x - PAGE_OFFSET) / PAGE_SIZE; \
		unsigned long __page = ((unsigned long)page_base + (__idx * (sizeof(struct page)))); \
		(struct page *)__page; \
		})

#define __page_to_virt(x) ({ \
		unsigned long __idx = (((unsigned long)x - (unsigned long)page_base) / sizeof(struct page)); \
		unsigned long __addr = ((__idx * PAGE_SIZE) + PAGE_OFFSET); \
		(void *)__addr; \
		})

#define virt_to_page __virt_to_page
#define page_to_virt __page_to_virt

static inline unsigned int buddy_order(struct page *page)
{
	return page->private;
}

static inline void set_page_private(struct page *page, unsigned long private)
{
    page->private = private;
}

/* used for memblock */
void memblock_free_pages(unsigned long pfn, unsigned int order);

/**
 * vmemmap page 初始化
 * 申请一片mem空间存放全部 mem 的page信息
 * 并初始化 struct page 的基础信息
 *
 */
int vmemmap_page_init(unsigned long mem_start, unsigned long mem_end);

/**
 * 初始化 buddy_zone info
 * 需要在调用 buddy free_pages/alloc_pages 接口之前完成初始化
 *
 */
int buddy_zone_init(void);

/**
 * alloc pages
 * return virt addr
 */
void *alloc_pages(unsigned int order);

/**
 * free pages
 * addr: virt addr
 *
 */
void free_pages(unsigned long addr, unsigned int order);
void buddyinfo_dump(void);

#endif
