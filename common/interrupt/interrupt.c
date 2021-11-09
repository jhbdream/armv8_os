#include <common/interrupt.h>
#include <errno.h>
#include <elog.h>
#include <compiler_types.h>
#include <driver/gic.h>

unsigned long task_interrupt_from_thread;
unsigned long task_interrupt_to_thread;
unsigned long task_thread_switch_interrupt_flag;

static inline void ack_bad_irq(unsigned int irq)
{
	log_e("unexpected IRQ trap at vector %02x", irq);
}

/**
 * handle_bad_irq - handle spurious and unhandled irqs
 * @desc:      description of the interrupt
 *
 * Handles spurious and unhandled IRQ's. It also prints a debugmessage.
 */
void handle_bad_irq(struct irq_desc *desc)
{
	unsigned int irq = irq_desc_get_irq(desc);
	ack_bad_irq(irq);
}

/* global irq desc */
struct irq_desc irq_desc[NR_IRQS] = {
	[0 ... NR_IRQS-1] = {
		.handle_irq	= handle_bad_irq,
		.depth		= 1,
	}
};

/*
 * Enter an interrupt context.
 */
void irq_enter(void)
{

}

/*
 * Exit an interrupt context.
 */
void irq_exit(void)
{

}

struct irq_desc *irq_to_desc(unsigned int irq)
{
	return (irq < NR_IRQS) ? irq_desc + irq : NULL;
}

/*
 * Architectures call this to let the generic IRQ layer
 * handle an interrupt.
 */
static inline void generic_handle_irq_desc(struct irq_desc *desc)
{
	desc->handle_irq(desc);
}

/**
 * generic_handle_irq - Invoke the handler for a particular irq
 * @irq:	The irq number to handle
 *
 */
int generic_handle_irq(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	generic_handle_irq_desc(desc);

	return 0;
}

void handle_domain_irq(struct pt_regs *regs)
{
	static uint32_t irqnr;
	irq_enter();
	irqnr = irq_read_iar();

	generic_handle_irq(irqnr);

	irq_eoi(irqnr);
	irq_exit();
}

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
					const char *name, void *data)
{
	struct irq_desc *desc;

	desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	if(handler != NULL)
	{
		desc->handle_irq = handler;
	}

	desc->flags = flags;
	desc->name = name;
	desc->handler_data = data;

	/* Reset broken irq detection when installing new handler */
	desc->irq_count = 0;

	irq_unmask(irq);

	return 0;
}
