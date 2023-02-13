#include <driver/interrupt.h>
#include <io.h>
#include <printk.h>
#include <stddef.h>

#include "asm/csr.h"
#include "plic.h"

#define PLIC_PRIORITY_BASE 0x0
#define PLIC_PENDING_BASE 0x1000
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000

static void *plic_regs;

static void plic_toggle(void *base, int hwirq, int enable)
{
	u32 *reg = base + PLIC_ENABLE_BASE + (hwirq / 32) * sizeof(u32);
	u32 hwirq_mask = 1 << (hwirq % 32);

	if (enable) {
		writel(readl(reg) | hwirq_mask, reg);
	} else {
		writel(readl(reg) & ~hwirq_mask, reg);
	}
}

static void plic_set_prio(void *base, int hwirq, int prio)
{
	u32 *reg = base + PLIC_PRIORITY_BASE + hwirq * 4;
	writel(prio, reg);
}

static void plic_eoi(void *base, int hwirq)
{
	u32 *reg = base + PLIC_CONTEXT_BASE + 0x04;
	writel(hwirq, reg);
}

static u32 plic_claim(void *base)
{
	u32 *reg = base + PLIC_CONTEXT_BASE + 0x04;
	return readl(reg);
}

void plic_init(void *base, int nr_irqs, int nr_contexts)
{
	int i;
	plic_regs = base;
	u32 threshold = 0;
	int hwirq;
	void *reg;

	for (i = 0; i < nr_contexts; i++) {
		reg = base + PLIC_ENABLE_BASE + i * 0x1000 + 0x00;
		writel(threshold, reg);

		for (hwirq = 1; hwirq < nr_irqs; hwirq++) {
			plic_set_prio(base, hwirq, 0xFF);
			plic_toggle(base, hwirq, 0);
		}
	}

	/* enable extern irq */
	csr_set(mie, MIE_MEIE);

	printk("plic: init done!\n");
}

void plic_irq_mask(unsigned int hwirq)
{
	plic_toggle(plic_regs, hwirq, 0);
}

void plic_irq_unmask(unsigned int hwirq)
{
	plic_toggle(plic_regs, hwirq, 1);
}

/* TODO: only support 1 core */
void plic_irq_eoi(unsigned int hwirq)
{
	plic_eoi(plic_regs, hwirq);
}

unsigned int plic_irq_claim(void)
{
	return plic_claim(plic_regs);
}

struct irq_chip riscv_plic_chip = {
	.irq_mask = plic_irq_mask,
	.irq_unmask = plic_irq_unmask,
	.irq_eoi = plic_irq_eoi,
};

static void riscv_handle_irq(void *reg)
{
	uint32_t irqnr = (uint32_t)plic_irq_claim();

	generic_handle_irq(irqnr);

	plic_irq_eoi(irqnr);
}

void riscv_plic_init(void)
{
	set_irq_chip(&riscv_plic_chip);
	set_handle_irq(riscv_handle_irq);

	/* hw init */
	plic_init((void *)0x60000000, 128, 1);
}
