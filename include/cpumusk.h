/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_CPUMASK_H
#define __LINUX_CPUMASK_H

#include <type.h>
#include <config.h>

/* Don't assign or return these: may not be this big! */
typedef struct cpumask {
	DECLARE_BITMAP(bits, NR_CPUS);
} cpumask_t;

/**
 * cpumask_bits - get the bits in a cpumask
 * @maskp: the struct cpumask *
 *
 * You should only assume nr_cpu_ids bits of this mask are valid.  This is
 * a macro so it's const-correct.
 */
#define cpumask_bits(maskp) ((maskp)->bits)

#endif /* __LINUX_CPUMASK_H */