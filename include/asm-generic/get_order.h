#ifndef __ASM_GENERIC_GETORDER_H
#define __ASM_GENERIC_GETORDER_H

#include <compiler_types.h>
#include <stddef.h>
#include <pgtable.h>

static inline int get_order(unsigned long size)
{
	int order;

    size = (size - 1) >> (PAGE_SHIFT - 1);
    order = -1;
    do
	{
		size >>= 1;
        order++;
    } while (size);

	return order;
}

#endif
