#ifndef __ASM_PAGE_DEF_H
#define __ASM_PAGE_DEF_H

#include <const.h>

#define PAGE_SHIFT (12)
#define PAGE_SIZE (_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

#endif
