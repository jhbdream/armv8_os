#include "driver/interrupt.h"
#include "printk.h"
#include <asm/asm.h>
#include <asm/csr.h>
#include <stddef.h>

/*
 * Possible interrupt causes:
 */
#define INTERRUPT_CAUSE_SOFTWARE	IRQ_M_SOFT
#define INTERRUPT_CAUSE_TIMER		IRQ_M_TIMER
#define INTERRUPT_CAUSE_EXTERNAL	IRQ_M_EXT

void trap_irq(unsigned long mcause)
{
	mcause &= ~(1UL << (__riscv_xlen - 1));
	switch (mcause) {
	case INTERRUPT_CAUSE_SOFTWARE:
		break;

	case INTERRUPT_CAUSE_TIMER:
		/* TODO: add clock evnet */
		printk("timer irq!\n");
		break;

	case INTERRUPT_CAUSE_EXTERNAL:
		handle_domain_irq(NULL);
		break;
	};

	return;
}

void trap_handler(unsigned long mcause, unsigned long mtval)
{
	/* irq handle */
	if (mcause & (1UL << (__riscv_xlen - 1)))
	{
		trap_irq(mcause);
	}

	/* exception handle */
	switch(mcause)
	{

	}
}
