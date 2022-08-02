#ifndef __RB_TREE_H__
#define __RB_TREE_H__

#include <container_of.h>

struct rb_node
{
#define RB_RED 0
#define RB_BLACK 1
	unsigned long rb_parent_color;
	struct rb_node *rb_right;
	struct rb_node *rb_left;
};

struct rb_root
{
	struct rb_node *rb_node;
};

#define RB_ROOT (struct rb_root) { NULL, }

#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
								struct rb_node **rb_link)
{
	node->rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

void rb_insert_color(struct rb_node *node, struct rb_root *root);

#endif
