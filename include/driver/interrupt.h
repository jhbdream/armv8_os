/* SPDX-License-Identifier: GPL-2.0 */
/* interrupt.h */
#ifndef _LINUX_INTERRUPT_H
#define _LINUX_INTERRUPT_H

#include <irqflags.h>

#define NR_IRQS 8192

/*
 * irq_get_irqchip_state/irq_set_irqchip_state specific flags
 */
enum irqchip_irq_state
{
    IRQCHIP_STATE_PENDING,    /* Is interrupt pending? */
    IRQCHIP_STATE_ACTIVE,     /* Is interrupt in progress? */
    IRQCHIP_STATE_MASKED,     /* Is interrupt masked? */
    IRQCHIP_STATE_LINE_LEVEL, /* Is IRQ line high? */
};

/*
 * IRQ line status.
 *
 * Bits 0-7 are the same as the IRQF_* bits in linux/interrupt.h
 *
 * IRQ_TYPE_NONE		- default, unspecified type
 * IRQ_TYPE_EDGE_RISING		- rising edge triggered
 * IRQ_TYPE_EDGE_FALLING	- falling edge triggered
 * IRQ_TYPE_EDGE_BOTH		- rising and falling edge triggered
 * IRQ_TYPE_LEVEL_HIGH		- high level triggered
 * IRQ_TYPE_LEVEL_LOW		- low level triggered
 * IRQ_TYPE_LEVEL_MASK		- Mask to filter out the level bits
 * IRQ_TYPE_SENSE_MASK		- Mask for all the above bits
 * IRQ_TYPE_DEFAULT		- For use by some PICs to ask irq_set_type
 *				  to setup the HW to a sane default (used
 *                                by irqdomain map() callbacks to synchronize
 *                                the HW state and SW flags for a newly
 *                                allocated descriptor).
 *
 * IRQ_TYPE_PROBE		- Special flag for probing in progress
 *
 * Bits which can be modified via irq_set/clear/modify_status_flags()
 * IRQ_LEVEL			- Interrupt is level type. Will be also
 *				  updated in the code when the above trigger
 *				  bits are modified via irq_set_irq_type()
 * IRQ_PER_CPU			- Mark an interrupt PER_CPU. Will protect
 *				  it from affinity setting
 * IRQ_NOPROBE			- Interrupt cannot be probed by autoprobing
 * IRQ_NOREQUEST		- Interrupt cannot be requested via
 *				  request_irq()
 * IRQ_NOTHREAD			- Interrupt cannot be threaded
 * IRQ_NOAUTOEN			- Interrupt is not automatically enabled in
 *				  request/setup_irq()
 * IRQ_NO_BALANCING		- Interrupt cannot be balanced (affinity set)
 * IRQ_MOVE_PCNTXT		- Interrupt can be migrated from process context
 * IRQ_NESTED_THREAD		- Interrupt nests into another thread
 * IRQ_PER_CPU_DEVID		- Dev_id is a per-cpu variable
 * IRQ_IS_POLLED		- Always polled by another interrupt. Exclude
 *				  it from the spurious interrupt detection
 *				  mechanism and from core side polling.
 * IRQ_DISABLE_UNLAZY		- Disable lazy irq disable
 */
enum
{
    IRQ_TYPE_NONE = 0x00000000,
    IRQ_TYPE_EDGE_RISING = 0x00000001,
    IRQ_TYPE_EDGE_FALLING = 0x00000002,
    IRQ_TYPE_EDGE_BOTH = (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING),
    IRQ_TYPE_LEVEL_HIGH = 0x00000004,
    IRQ_TYPE_LEVEL_LOW = 0x00000008,
    IRQ_TYPE_LEVEL_MASK = (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH),
    IRQ_TYPE_SENSE_MASK = 0x0000000f,
    IRQ_TYPE_DEFAULT = IRQ_TYPE_SENSE_MASK,

    IRQ_TYPE_PROBE = 0x00000010,

    IRQ_LEVEL = (1 << 8),
    IRQ_PER_CPU = (1 << 9),
    IRQ_NOPROBE = (1 << 10),
    IRQ_NOREQUEST = (1 << 11),
    IRQ_NOAUTOEN = (1 << 12),
    IRQ_NO_BALANCING = (1 << 13),
    IRQ_MOVE_PCNTXT = (1 << 14),
    IRQ_NESTED_THREAD = (1 << 15),
    IRQ_NOTHREAD = (1 << 16),
    IRQ_PER_CPU_DEVID = (1 << 17),
    IRQ_IS_POLLED = (1 << 18),
    IRQ_DISABLE_UNLAZY = (1 << 19),
};


struct irq_desc;
typedef void (*irq_handler_t)(struct irq_desc *desc);

struct irq_chip
{
    void (*irq_mask)(unsigned int irq);
    void (*irq_unmask)(unsigned int irq);
    void (*irq_eoi)(unsigned int irq);
};

/**
 * struct irq_desc - interrupt descriptor
 * @handle_irq:		highlevel irq-events handler
 * @irq:			hw irq num
 * @status_use_accessors: status information
 * @depth:		disable-depth, for nested irq_disable() calls
 * @tot_count:		stats field for non-percpu irqs
 * @irq_count:		stats field to detect stalled irqs
 * @name:		flow handler name for /proc/interrupts output
 */
struct irq_desc
{
    irq_handler_t handle_irq;
    unsigned int irq;
    unsigned int flags;
    unsigned int status_use_accessors;
    unsigned int depth;     /* nested irq disables */
    unsigned int irq_count; /* For detecting broken IRQs */
    const char *name;
    void *handler_data;
};

extern struct irq_desc irq_desc[NR_IRQS];

static inline unsigned int irq_desc_get_irq(struct irq_desc *desc)
{
    return desc->irq;
}

void handle_domain_irq(void *regs);
int set_irq_chip(struct irq_chip *irq_chip);
int set_handle_irq(void (*handle_irq)(void *));
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
                const char *name, void *data);

int is_interrupt_nest(void);
int generic_handle_irq(unsigned int irq);

#endif
