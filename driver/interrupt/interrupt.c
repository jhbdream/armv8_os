#include <driver/interrupt.h>
#include <stdarg.h>
#include <stddef.h>
#include <errno.h>
#include <compiler_types.h>

unsigned long task_interrupt_from_thread;
unsigned long task_interrupt_to_thread;
unsigned long task_thread_switch_interrupt_flag;

unsigned long interrupt_nest = 0;

int is_interrupt_nest(void)
{
	return (interrupt_nest > 0) ? 1 : 0;
}

/**
 * @brief 无效中断入口
 *
 * @param val
 */
void unexpected_exception(int val)
{
	while (1) {
		/* code */
	}
}

static inline void ack_bad_irq(unsigned int irq)
{
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

struct irq_chip _irq_chip = {
	.irq_eoi = NULL,
	.irq_mask = NULL,
	.irq_unmask = NULL,
};

/* global irq desc */
struct irq_desc irq_desc[NR_IRQS] = { [0 ... NR_IRQS - 1] = {
					      .handle_irq = handle_bad_irq,
					      .depth = 1,
				      } };

/*
 * Enter an interrupt context.
 */
void irq_enter(void)
{
	interrupt_nest++;
}

/*
 * Exit an interrupt context.
 */
void irq_exit(void)
{
	interrupt_nest--;
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
 * 由中断控制器的handle调用该接口
 *
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

void (*handle_arch_irq)(void *);

int set_irq_chip(struct irq_chip *irq_chip)
{
	if (!irq_chip) {
		return -EBUSY;
	}

	_irq_chip.irq_unmask = irq_chip->irq_unmask;
	_irq_chip.irq_mask = irq_chip->irq_mask;
	_irq_chip.irq_eoi = irq_chip->irq_eoi;

	return 0;
}

struct irq_chip *get_irq_chip(void)
{
	return &_irq_chip;
}

int set_handle_irq(void (*handle_irq)(void *))
{
	if (handle_arch_irq) {
		return -EBUSY;
	}

	handle_arch_irq = handle_irq;
	return 0;
}

void handle_domain_irq(void *regs)
{
	irq_enter();

	if (handle_arch_irq)
		handle_arch_irq(regs);

	irq_exit();
}

void mask_irq(unsigned int irq)
{
	struct irq_chip *chip = get_irq_chip();

	if (chip->irq_mask)
		chip->irq_mask(irq);
}

void unmask_irq(unsigned int irq)
{
	struct irq_chip *chip = get_irq_chip();

	if (chip->irq_unmask)
		chip->irq_unmask(irq);
}

static int _setup_irq(unsigned int irq)
{
	unmask_irq(irq);
	return 0;
}

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
		const char *name, void *data)
{
	struct irq_desc *desc;

	desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	if (handler != NULL) {
		desc->handle_irq = handler;
	}

	desc->flags = flags;
	desc->name = name;
	desc->handler_data = data;

	/* Reset broken irq detection when installing new handler */
	desc->irq_count = 0;

	/* 申请中断意味着会取消中断控制器的屏蔽 */
	_setup_irq(irq);
	return 0;
}
