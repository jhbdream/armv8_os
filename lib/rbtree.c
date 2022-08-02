#include "stddef.h"
#include <ee/rbtree.h>

void rb_insert_color(struct rb_node *node, struct rb_root *root)
{

}

/* --------------------------------------------------------------------------*/
/**
 * @brife  找到根据大小排序的前一个值
 *
 * @Param node
 *
 * @Returns
 */
/* ----------------------------------------------------------------------------*/
struct rb_node *rb_prev(struct rb_node *node)
{
	struct rb_node *parent;

	if(rb_parent(node) == node)
	{
		return NULL;
	}

	/* 左 右 右 右... */
	if(node->rb_left)
	{
		node = node->rb_left;

		while(node->rb_right)
		{
			node = node->rb_right;
		}

		return node;
	}

	/* up up up */
	while((parent = rb_parent(node)) && (node == parent->rb_left))
	{
		node = parent;
	}

	return parent;
}



