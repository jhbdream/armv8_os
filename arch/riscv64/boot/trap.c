#include "driver/interrupt.h"
#include "printk.h"
#include <asm/asm.h>
#include <asm/csr.h>
#include <stddef.h>

/* exception cause */
#define CAUSE_MISALIGNED_FETCH 0x0
#define CAUSE_FETCH_ACCESS 0x1
#define CAUSE_ILLEGAL_INSTRUCTION 0x2
#define CAUSE_BREAKPOINT 0x3
#define CAUSE_MISALIGNED_LOAD 0x4
#define CAUSE_LOAD_ACCESS 0x5
#define CAUSE_MISALIGNED_STORE 0x6
#define CAUSE_STORE_ACCESS 0x7
#define CAUSE_USER_ECALL 0x8
#define CAUSE_HYPERVISOR_ECALL 0x9
#define CAUSE_SUPERVISOR_ECALL 0xa
#define CAUSE_MACHINE_ECALL 0xb
#define CAUSE_FETCH_PAGE_FAULT 0xc
#define CAUSE_LOAD_PAGE_FAULT 0xd
#define CAUSE_STORE_PAGE_FAULT 0xf
#define CAUSE_FETCH_GUEST_PAGE_FAULT 0x14
#define CAUSE_LOAD_GUEST_PAGE_FAULT 0x15
#define CAUSE_STORE_GUEST_PAGE_FAULT 0x17

void trap_irq(unsigned long mcause)
{
	mcause &= ~(1UL << (__riscv_xlen - 1));
	switch (mcause) {
	case IRQ_U_SOFT:
		break;

	case IRQ_S_SOFT:
		break;

	case IRQ_M_SOFT:
		break;

	case IRQ_U_TIMER:
		break;

	case IRQ_S_TIMER:
		break;

	case IRQ_M_TIMER:
		/* TODO: add clock evnet */
		printk("timer irq!\n");
		break;

	case IRQ_U_EXT:
		break;

	case IRQ_S_EXT:
		break;

	case IRQ_M_EXT:
		handle_domain_irq(NULL);
		break;
	};

	return;
}

void trap_handler(unsigned long mcause, unsigned long mtval)
{
	/* irq handle */
	if (mcause & (1UL << (__riscv_xlen - 1))) {
		trap_irq(mcause);
		return;
	}

	/* exception handle */
	switch (mcause) {
	case CAUSE_MISALIGNED_FETCH:
		break;
	case CAUSE_FETCH_ACCESS:
		break;
	case CAUSE_ILLEGAL_INSTRUCTION:
		break;
	case CAUSE_BREAKPOINT:
		break;
	case CAUSE_MISALIGNED_LOAD:
		break;
	case CAUSE_LOAD_ACCESS:
		break;
	case CAUSE_MISALIGNED_STORE:
		break;
	case CAUSE_STORE_ACCESS:
		break;
	case CAUSE_USER_ECALL:
		break;
	case CAUSE_HYPERVISOR_ECALL:
		break;
	case CAUSE_SUPERVISOR_ECALL:
		break;
	case CAUSE_MACHINE_ECALL:
		break;
	case CAUSE_FETCH_PAGE_FAULT:
		break;
	case CAUSE_LOAD_PAGE_FAULT:
		break;
	case CAUSE_STORE_PAGE_FAULT:
		break;
	case CAUSE_FETCH_GUEST_PAGE_FAULT:
		break;
	case CAUSE_LOAD_GUEST_PAGE_FAULT:
		break;
	case CAUSE_STORE_GUEST_PAGE_FAULT:
		break;
	}

	return;
}
