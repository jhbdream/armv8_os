/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_BUG_H
#define _ASM_GENERIC_BUG_H

#include <printk.h>
#include <compiler_types.h>

#ifndef HAVE_ARCH_BUG
#define BUG()                                                                  \
	do {                                                                   \
		printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__,    \
		       __func__);                                              \
		barrier_before_unreachable();                                  \
		while (1)                                                      \
			;                                                      \
	} while (0)
#endif

#ifndef HAVE_ARCH_BUG_ON
#define BUG_ON(condition)                                                      \
	do {                                                                   \
		if (unlikely(condition))                                       \
			BUG();                                                 \
	} while (0)
#endif

#endif
