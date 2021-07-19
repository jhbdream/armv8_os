#ifndef __ASM_IRQFLAGS_H
#define __ASM_IRQFLAGS_H

/*
 * CPU interrupt mask handling.
 */
static inline void arch_local_irq_enable(void)
{
	asm volatile( "msr daifclr, #3" ::: "memory");
}

static inline void arch_local_irq_disable(void)
{
	asm volatile( "msr daifset, #3" ::: "memory");
}

#endif