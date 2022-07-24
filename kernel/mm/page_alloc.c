#include "errno.h"
#include "mm/memblock.h"
#include "printk.h"
#include "type.h"
#include <mm/page_alloc.h>
#include <mm/page_flag.h>
#include <ee/pgtable.h>
#include <round.h>
#include <asm-generic/bug.h>
#include <list.h>

/* use buddy page alloc */
struct zone _zone;
struct page *page_base = NULL;

static struct zone *zone_get(void)
{
	return &_zone;
}
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

	/* do some page base initial */
	for(unsigned long i = 0; i < page_count; i++)
	{
		page->page_type = -1;
		page->private = 0;
		page++;
	}

	return ret;
}

/*
 * Locate the struct page for both the matching buddy in our
 * pair (buddy1) and the combined O(n+1) page they form (page).
 *
 * 1) Any buddy B1 will have an order O twin B2 which satisfies
 * the following equation:
 *     B2 = B1 ^ (1 << O)
 * For example, if the starting buddy (buddy2) is #8 its order
 * 1 buddy is #10:
 *     B2 = 8 ^ (1 << 1) = 8 ^ 2 = 10
 *
 * 2) Any buddy B will have an order O+1 parent P which
 * satisfies the following equation:
 *     P = B & ~(1 << O)
 *
 * Assumption: *_mem_map is contiguous at least up to MAX_ORDER
 */
static inline unsigned long
__find_buddy_pfn(unsigned long page_pfn, unsigned int order)
{
    return page_pfn ^ (1 << order);
}

static inline void set_buddy_order(struct page *page, unsigned int order)
{
    set_page_private(page, order);
    __SetPageBuddy(page);
}


/*
 * This function checks whether a page is free && is the buddy
 * we can coalesce a page and its buddy if
 * (a) the buddy is not in a hole (check before calling!) &&
 * (b) the buddy is in the buddy system &&
 * (c) a page and its buddy have the same order &&
 * (d) a page and its buddy are in the same zone.
 *
 * For recording whether a page is in the buddy system, we set PageBuddy.
 * Setting, clearing, and testing PageBuddy is serialized by zone->lock.
 *
 * For recording page's order, we use page_private(page).
 */
static inline bool page_is_buddy(struct page *buddy,
                            unsigned int order)
{
	if(!PageBuddy(buddy))
		return false;

    if (buddy_order(buddy) != order)
        return false;

    return true;
}

/* Used for pages not on another list */
static inline void add_to_free_list(struct page *page,
                    unsigned int order)
{
	struct free_area *area = &zone_get()->free_area[order];

	list_add(&page->node, &area->free_list);
    area->nr_free++;

	set_buddy_order(page, order);
}

/**
 * 删除一个buddy page
 * 1. 从链表移除
 * 2. 清除 buddy flag
 * 3. 清除 private 内容
 * 4. 修改 nr_free 计数
 *
 */
static inline void del_page_from_free_list(struct page *page, unsigned int order)
{
    list_del(&page->node);
    __ClearPageBuddy(page);
	set_page_private(page, 0);
	zone_get()->free_area[order].nr_free--;
}

int buddy_init(void)
{
	int ret = 0;

	struct zone *zone = zone_get();

	for(int i = 0; i < MAX_ORDER; i++)
	{
		zone->free_area[i].nr_free = 0;
		INIT_LIST_HEAD(&zone->free_area[i].free_list);
	}

	return ret;
}

void free_pages(struct page *page, unsigned int order)
{
	unsigned long pfn;
	unsigned long buddy_pfn;
	struct page *buddy;

	pfn = page_to_pfn(page);

	/**
	 *  order: 0 - 10; 循环是为了把小的阶合并成大的阶,
	 *  如果 order = max_order - 1 就不需要进行循环 直接挂到链表
	 *
	 *  所以这里的循环范围是到 max_order - 1
	 */
	while(order < (MAX_ORDER - 1))
	{
		/* 计算该page的伙伴 pfb */
		buddy_pfn = __find_buddy_pfn(pfn, order);
		buddy = page + (buddy_pfn - pfn);

		/* 没有合适的伙伴的话 加入当前阶的链表 */
		if(!page_is_buddy(buddy, order))
		{
			break;
		}

		/* 如果找到伙伴、合并并升级阶  */
		/**
		 *  1. page = 12, order = 2;
		 *	2. find buddy page 8;
		 *
		 *  0		4		8		12		16
		 *	+-------+-------+-------+-------+-------+-------+
		 *  |				| free	| free	|				|
		 *  +-------+-------+-------+-------+-------+-------+
		 *					buddy   page
		 *
		 *  3. merge page 12 && buddy page 8 to new page 8 order = 3;
		 *
		 *  0		4		8		12		16
		 *	+-------+-------+-------+-------+-------+-------+
		 *  |				|		free	|				|
		 *  +-------+-------+-------+-------+-------+-------+
		 *					new_page
		 *
		 * a. remove buddy_page from list
		 * b. get new pfn pfn ^ buddy_pfn
		 * c.
		 */

		del_page_from_free_list(page, order);

		unsigned long combined_pfn = pfn & buddy_pfn;
		page = page + (combined_pfn - pfn);
		pfn = combined_pfn;
		order++;
	}

	/**
	 * 将 page 添加到新的阶段链表
	 */
	add_to_free_list(page, order);
}

/* 将 membloc 空闲内存加入到buddy */
void memblock_free_pages(unsigned long pfn, unsigned int order)
{
	struct page *page = pfn_to_page(pfn);

	printk("%s: pfn start[0x%x]  order[%d]\n", __func__, pfn, order);
	free_pages(page, order);
}
