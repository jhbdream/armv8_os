#include "rbtree.h"
#include "type.h"
#include <eeos.h>
#include <stddef.h>
#include <asm-generic/bug.h>

struct vmap_area
{
	unsigned long va_start;
	unsigned long va_end;
	unsigned long flags;
	struct rb_node rb_node;
	struct list_head list;
};

static struct rb_root vmap_area_root = RB_ROOT;

/* --------------------------------------------------------------------------*/
/**
 * @brife  从 红黑二叉树 中找到起始地址对应区域
 *
 * @Param addr
 *
 * @Returns
 */
/* ----------------------------------------------------------------------------*/
static struct vmap_area *__find_vmap_area(unsigned long addr)
{
	struct rb_node *n = vmap_area_root.rb_node;

	while(n)
	{
		struct vmap_area *va;
		va = rb_entry(n, struct vmap_area, rb_node);

		if(addr < va->va_start)
		{
			n = n->rb_left;
		}
		else if (addr > va->va_start)
		{
			n = n->rb_right;
		}
		else
		{
			return va;
		}
	}

	return NULL;
}


/* --------------------------------------------------------------------------*/
/**
 * @brife  将区域插入到红黑树中
 *
 * @Param area
 */
/* ----------------------------------------------------------------------------*/
static void __insert_vmap_area(struct vmap_area *va)
{
	struct rb_node **p = &vmap_area_root.rb_node;
	struct rb_node *parent = NULL;
	struct rb_node *tmp;

	/* 找到要插入位置 */
	while(*p)
	{
		struct vmap_area *va_tmp;

		parent = *p;
		va_tmp = rb_entry(parent, struct vmap_area, rb_node);

		if(va->va_start < va_tmp->va_end)
		{
			p = &((*p)->rb_left);
		}
		else if(va->va_end > va_tmp->va_start)
		{
			p = &((*p)->rb_right);
		}
		else
		{
			BUG();
		}
	}

	/* 插入节点 */
	rb_link_node(&va->rb_node, parent, p);
	rb_insert_color(&va->rb_node, &vmap_area_root);

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
















