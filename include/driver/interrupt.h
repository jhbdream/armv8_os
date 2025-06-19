/* SPDX-License-Identifier: GPL-2.0 */
/* interrupt.h */
#ifndef _LINUX_INTERRUPT_H
#define _LINUX_INTERRUPT_H

#define NR_IRQS 8192

struct irq_desc;

typedef void (*irq_handler_t)(struct irq_desc *desc);

struct irq_chip {
    void (*irq_mask)(unsigned int irq);
    void (*irq_unmask)(unsigned int irq);
};

/**
 * struct irq_desc - interrupt descriptor
 * @handle_irq:		highlevel irq-events handler
 * @irq:			hw irq num
 * @depth:		disable-depth, for nested irq_disable() calls
 * @tot_count:		stats field for non-percpu irqs
 * @irq_count:		stats field to detect stalled irqs
 * @name:		flow handler name for /proc/interrupts output
 */
struct irq_desc {
    irq_handler_t handle_irq;
    unsigned int  irq;
    unsigned int  flags;
    unsigned int  depth;     /* nested irq disables */
    unsigned int  irq_count; /* For detecting broken IRQs */
    const char   *name;
    void         *handler_data;
};

static inline unsigned int irq_desc_get_irq(struct irq_desc *desc)
{
    return desc->irq;
}

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
                const char *name, void *data);

int set_irq_chip(struct irq_chip *irq_chip);
int set_handle_irq(void (*handle_irq)(void *));
int generic_handle_irq(unsigned int irq);

int is_interrupt_nest(void);

#endif
