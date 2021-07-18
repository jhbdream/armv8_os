#include <config.h>
#include <common/interrupt.h>
#include "gicv3/arm-gic-v3.h"

int gic_init(void)
{
    gic_init_bases( (void *)GIC_DIST_BASE_ADDR,
                (void *)GIC_RD_BASE_ADDR,
                NR_CPUS);
}

void irq_mask(uint32_t hwirq)
{
    gic_mask_irq(hwirq);
}

void irq_unmask(uint32_t hwirq)
{
    gic_unmask_irq(hwirq);
}

uint32_t irq_read_iar(void)
{
    return gic_read_iar();
}

void irq_eoi(uint32_t hwirq)
{
	gic_eoi_irq(hwirq);
}
