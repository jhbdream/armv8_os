#ifndef __ASM_IRQFLAGS_H
#define __ASM_IRQFLAGS_H

/*
 * CPU interrupt mask handling.
 */
static inline void arch_local_irq_enable(void)
{
    asm volatile("msr daifclr, #2" ::
                     : "memory");
}

static inline void arch_local_irq_disable(void)
{
    asm volatile("msr daifset, #2" ::
                     : "memory");
}

static inline unsigned long arch_local_irq_save(void)
{
    unsigned long flags;
    asm volatile(
        "mrs	%0, daif		// arch_local_irq_save\n"
        "msr	daifset, #2"
        : "=r"(flags)
        :
        : "memory");
    return flags;
}

/*
 * restore saved IRQ state
 */
static inline void arch_local_irq_restore(unsigned long flags)
{
    asm volatile(
        "msr	daif, %0		// arch_local_irq_restore"
        :
        : "r"(flags)
        : "memory");
}

#endif