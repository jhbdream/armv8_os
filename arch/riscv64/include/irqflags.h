#ifndef __ASM_IRQFLAGS_H
#define __ASM_IRQFLAGS_H

/*
 * CPU interrupt mask handling.
 */
static inline void arch_local_irq_enable(void)
{
}

static inline void arch_local_irq_disable(void)
{
}

static inline unsigned long arch_local_irq_save(void)
{
}

/*
 * restore saved IRQ state
 */
static inline void arch_local_irq_restore(unsigned long flags)
{
}

#endif
