#include "rbtree.h"
#include "type.h"
#include <eeos.h>
#include <stddef.h>

struct vmap_area
{
	unsigned long va_start;
	unsigned long va_end;
	unsigned long flags;
	struct rb_node rb_node;
	struct list_head list;
};

static struct vmap_area *__find_vmap_area(unsigned long addr)
{
	return NULL;
}

static void __insert_vmap_area(struct vmap_area *area)
{

}

static struct vmap_area *alloc_vmap_area(unsigned long size, unsigned long align,
									unsigned long vstart, unsigned long vend)
{
	return NULL;
}

static void free_vmap_area(struct vmap_area *area)
{

}

void vmalloc_init(void)
{

}
















