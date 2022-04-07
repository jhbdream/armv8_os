#ifndef __ASM_IRQFLAGS_H
#define __ASM_IRQFLAGS_H

#include <asm/csr.h>

/*
 * CPU interrupt mask handling.
 */
static inline void arch_local_irq_enable(void)
{
	csr_set(mstatus, 0x8);
}

static inline void arch_local_irq_disable(void)
{
	csr_clear(mstatus, 0x8);
}

static inline unsigned long arch_local_irq_save(void)
{
	return csr_read_clear(mstatus, 0x8);
}

static inline void arch_local_irq_restore(unsigned long flags)
{
	csr_set(mstatus, flags & 0x8);
}

#endif
