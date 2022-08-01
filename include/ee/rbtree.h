#ifndef __RB_TREE_H__
#define __RB_TREE_H__

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

#endif
