#include "printk.h"
#include "type.h"
#include <stddef.h>
#include <stdint.h>
#include <list.h>
#include <mm/page_alloc.h>
#include <round.h>
#include <asm/pgtable.h>
#include <ee/align.h>
#include <asm-generic/bug.h>
#include <asm-generic/get_order.h>
#include <mm/page_flag.h>
#include <mm/slab.h>

typedef s16 slobidx_t;

struct slob_block {
	slobidx_t units;
};
typedef struct slob_block slob_t;

#define SLOB_ALIGN 8
#define SLOB_ALIGN_OFFSET 8

#define SLOB_ORDER 0
#define SLOB_BREAK_SMALL 256
#define SLOB_BREAK_MEDIUM 1024

LIST_HEAD(small_slob);
LIST_HEAD(medium_slob);
LIST_HEAD(large_slob);

#define SLOB_UNIT sizeof(slob_t)
#define SLOB_UNITS(size) DIV_ROUND_UP(size, SLOB_UNIT)

/* --------------------------------------------------------------------------*/
/**
 * @brife  在空闲slab中存放一些标记,让空间单元链接成一个单链表结构
 *
 *
 * if(size > 1) use two unit save size && offset to next
 * +-------+-------+-------+-------+-------+-------+
 * |size   |offset |free   |free   |	   |	   |
 * +-------+-------+-------+-------+-------+-------+
 *				|				   |
 *				|				   |
 *				+------------------+
 *
 * if(size == 1) not store size, user -offfset point to next
 * +-------+-------+-------+-------+-------+-------+
 * |-offset|-offset|	   |	   |	   |	   |
 * +-------+-------+-------+-------+-------+-------+
 *
 * next = cur + offset
 *
 * @Param s
 * @Param size
 * @Param next
 */
/* ----------------------------------------------------------------------------*/
static void set_slob(slob_t *s, slobidx_t size, slob_t *next)
{
	slob_t *base = (slob_t *)((unsigned long)s & PAGE_MASK);
	slobidx_t offset = (next - base);
	if(size > 1)
	{
		s[0].units = size;
		s[1].units = offset;
	}
	else
	{
		s[0].units = -offset;
	}
}

static slobidx_t slob_units(slob_t *s)
{
	if(s->units > 0)
	{
		return s->units;
	}
	else
	{
		return 1;
	}
}

static slob_t *slob_next(slob_t *s)
{
	slob_t *base = (slob_t *)((unsigned long)s & PAGE_MASK);

	if(s->units > 0)
	{
		return base + s[1].units;
	}
	else
	{
		return base + (-s->units);
	}
}

static void *slob_new_page(int order)
{
	struct page *page;
	page = alloc_pages(order);
	__SetPageSlab(page);
	return page_to_virt(page);
}

static int slab_last(slob_t *s)
{
	return !((unsigned long)slob_next(s) & ~PAGE_MASK);
}

static void *slob_page_alloc(struct page *slob_page, size_t size,
					int align, int align_offset, bool *page_remove_from_list)
{
	slob_t *prev, *cur, *aligned = NULL;
	int delta = 0;
	int units = SLOB_UNITS(size);

	*page_remove_from_list = false;

	for(prev = NULL, cur = slob_page->freelist; ; prev = cur, cur = slob_next(cur))
	{
		slobidx_t avail = slob_units(cur);

		if(align)
		{
			aligned = (slob_t *)
				((ALIGN((unsigned long)cur + align_offset, align)) - align_offset);

			delta = aligned - cur;
		}

		if(avail >= (delta + units))
		{
			slob_t *next;
			next = slob_next(cur);

			/* 拆分 slob */
			if(delta)
			{
				set_slob(cur, delta, aligned);
				set_slob(aligned, avail - delta, next);
				prev = cur;
				cur = aligned;
				avail = slob_units(cur);
			}

			if(avail == units)
			{
				if(prev)
				{
					set_slob(prev, slob_units(prev), next);
				}
				else
				{
					slob_page->freelist = next;
				}
			}
			else
			{
				if(prev)
				{
					set_slob(prev, slob_units(prev), cur + units);
				}
				else
				{
					slob_page->freelist = cur + units;
				}
				set_slob(cur + units, avail - units, next);
			}

			slob_page->units -= units;
			if(!slob_page->units)
			{
				*page_remove_from_list = true;
			}

			return cur;
		}

		if(slab_last(cur))
		{
			return NULL;
		}
	}
}

static void *slob_alloc(size_t size, int align, int align_offset)
{
	slob_t *b = NULL;
	struct page *slob_page;
	struct list_head *slob_list = NULL;

	if(size < SLOB_BREAK_SMALL)
	{
		slob_list = &small_slob;
	}
	else if(size < SLOB_BREAK_MEDIUM)
	{
		slob_list = &medium_slob;
	}
	else
	{
		slob_list = &large_slob;
	}

	bool page_remove_from_list;
	list_for_each_entry(slob_page, slob_list, slob_node)
	{
		/* page 内剩余空间是否够分配 */
		if(slob_page->units < SLOB_UNITS(size))
		{
			continue;
		}

		b = slob_page_alloc(slob_page, size, align, align_offset, &page_remove_from_list);
		if(b == NULL)
		{
			continue;
		}

		if(page_remove_from_list == true)
		{

		}

		break;
	}

	if(b == NULL)
	{
		b = slob_new_page(SLOB_ORDER);
		if(!b)
			return NULL;

		slob_page = virt_to_page(b);
		slob_page->freelist = b;
		slob_page->units = SLOB_UNITS(PAGE_SIZE);
		set_slob(b, SLOB_UNITS(PAGE_SIZE), b + SLOB_UNITS(PAGE_SIZE));

		INIT_LIST_HEAD(&slob_page->slob_node);
		list_add(&slob_page->slob_node, slob_list);

		b = slob_page_alloc(slob_page, size, align, align_offset, &page_remove_from_list);
	}

	return b;
}

static void slob_free_pages(void *block, int order)
{
	struct page *page = virt_to_page(block);
	__ClearPageSlab(page);
	__free_pages(page, order);
}

static void slob_free(void *block, int size)
{
	struct page *slob_page;
	slob_t *prev, *next, *b = (slob_t *)block;
	slobidx_t units = SLOB_UNITS(size);

	if(block == NULL)
		return;

	BUG_ON(!size);

	slob_page = virt_to_page(block);

	if((slob_page->units + units) == SLOB_UNITS(PAGE_SIZE))
	{
		slob_free_pages(block, SLOB_ORDER);
		return;
	}

	slob_page->units += units;

	/* 要释放的位置在前面 */
	if(b < (slob_t *)slob_page->freelist)
	{
		/* 是否可以合并块 */
		if(b + units == (slob_t *)slob_page->freelist)
		{
			units += slob_units(slob_page->freelist);
			slob_page->freelist = slob_next(slob_page->freelist);
		}
		set_slob(b, units, slob_page->freelist);
		slob_page->freelist = b;
	}
	else
	{
		prev = (slob_t *)slob_page->freelist;
		next = slob_next(prev);

		while(b > next)
		{
			prev = next;
			next = slob_next(prev);
		}

		if(!slab_last(prev) && (b + units == next))
		{
			units += slob_units(next);
			set_slob(b, units, slob_next(next));
		}
		else
		{
			set_slob(b, units, next);
		}

		if(prev + slob_units(prev) == b)
		{
			units = slob_units(prev) + slob_units(b);
			set_slob(prev, units, slob_next(b));
		}
		else
		{
			set_slob(prev, slob_units(prev), b);
		}
	}
}

void *__kmalloc(size_t size)
{
	void *m = NULL;

	if(size < PAGE_SIZE - SLOB_ALIGN)
	{
		if(!size)
			return NULL;

		m = slob_alloc(size + SLOB_ALIGN_OFFSET, SLOB_ALIGN, SLOB_ALIGN_OFFSET);
		if(!m)
			return NULL;

		*(unsigned long *)m = size;
		return m + SLOB_ALIGN_OFFSET;
	}
	else
	{
		int order = get_order(size);
		m = slob_new_page(order);
		return m;
	}
}

void __kfree(const void *block)
{
	if(!block)
		return;

	struct page *page = virt_to_page(block);

	if(PageSlab(page))
	{
		unsigned long *m = (unsigned long *)(block - SLOB_ALIGN_OFFSET);
		slob_free(m, *m + SLOB_ALIGN_OFFSET);
	}
	else
	{
		int order = compound_order(page);
		__free_pages(page, order);
	}
}

void slob_test(void)
{
#define SLOB_TEST_CYCLE 32
#define SLOB_TEST_SIZE 61

	void *addr[SLOB_TEST_CYCLE];
	int i;
	for(i = 0; i < SLOB_TEST_CYCLE; i++)
	{
		addr[i] = kmalloc(i);
		printk("[%d] kmalloc addr: [0x%016lx]\n", i, addr[i]);
	}

	for(i = 0; i < SLOB_TEST_CYCLE; i++)
	{
		kfree(addr[i]);
		printk("[%d] kfree addr: [0x%016lx]\n", i, addr[i]);
	}

	printk("slob test done!\n");
}
