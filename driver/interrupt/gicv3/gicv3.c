#include <driver/interrupt.h>

#include <type.h>
#include <io.h>
#include <asm/memory.h>

#include <printk.h>

#include "gicv3.h"

void *gicd_base;
void *gicr_base;
void *gicr_sgi_base;

static inline u32 icc_sre_el1(void)
{
    u32 x;
    asm volatile("mrs %0, S3_0_C12_C12_5" : "=r"(x));
    return x;
}

static inline void w_icc_sre_el1(u32 x)
{
    asm volatile("msr S3_0_C12_C12_5, %0" : : "r"(x));
}

static inline void w_icc_igrpen1_el1(u32 x)
{
    asm volatile("msr S3_0_C12_C12_7, %0" : : "r"(x));
}

static inline void w_icc_pmr_el1(u32 x)
{
    asm volatile("msr S3_0_C4_C6_0, %0" : : "r"(x));
}

static inline void w_icc_eoir1_el1(u32 x)
{
    asm volatile("msr S3_0_C12_C12_1, %0" : : "r"(x));
}

static inline u32 icc_iar1_el1(void)
{
    u32 x;
    asm volatile("mrs %0, S3_0_C12_C12_0" : "=r"(x));
    return x;
}

void gicv3_mask_irq(u32 hwirq)
{
    uint32_t mask;

    mask = 1 << (hwirq % 32);

    if (hwirq < GICV3_NR_LOCAL_IRQS) {
        writel(mask, gicr_sgi_base + GICR_ICENABLER + (hwirq / 32) * 4);
    } else {
        writel(mask, gicd_base + GICD_ICENABLER + (hwirq / 32) * 4);
    }
}

void gicv3_unmask_irq(u32 hwirq)
{
    uint32_t mask;

    mask = 1 << (hwirq % 32);

    if (hwirq < GICV3_NR_LOCAL_IRQS) {
        writel(mask, gicr_sgi_base + GICR_ISENABLER + (hwirq / 32) * 4);
    } else {
        writel(mask, gicd_base + GICD_ISENABLER + (hwirq / 32) * 4);
    }
}

void gicv3_eoi_irq(u32 hwirq)
{
    w_icc_eoir1_el1(hwirq);
}

u32 gic_read_iar(void)
{
    return icc_iar1_el1();
}

static struct irq_chip gicv3_chip = {
    .irq_mask   = gicv3_mask_irq,
    .irq_unmask = gicv3_unmask_irq,
};

static void gicv3_handle_irq(void *reg)
{
    u32 irqnr;

    irqnr = gic_read_iar();

    if ((irqnr >= 1020 && irqnr <= 1023)) {
        printk("irqnr invalid: [%d]\n", irqnr);
        return;
    }

    generic_handle_irq(irqnr);

    gicv3_eoi_irq(irqnr);
}

static void gicv3_gicd_wait_for_rwp(void *gicd_base)
{
    while (readl(gicd_base + GICD_CTLR) & (1 << 31))
        ;
}

int gicv3_hw_init(void *dist_base, void *rdist_base, uint32_t nr_redist_regions)
{
    int i;

    uint32_t type, nr_lines, pr;

    uint32_t gicv3_waker_value;

    gicd_base     = dist_base + DEVICE_START;
    gicr_base     = rdist_base + DEVICE_START;
    gicr_sgi_base = rdist_base + DEVICE_START + (64 * 1024);

    writel(0, gicd_base + GICD_CTLR);

    type     = readl(gicd_base + GICD_TYPER);
    nr_lines = 32 * ((type & 0x1f));

    printk("gicv3 typer: [0x%x] nr_lines: [%d]\n", type, nr_lines);

    /* default all golbal IRQS to level, active low */
    for (i = GICV3_NR_LOCAL_IRQS; i < nr_lines; i += 16) {
        writel(0, gicd_base + GICD_ICFGR + (i / 16) * 4);
    }

    /* default priority for global interrupts */
    for (i = GICV3_NR_LOCAL_IRQS; i < nr_lines; i += 4) {
        pr = (0xa0 << 24) | (0xa0 << 16) | (0xa0 << 8) | 0xa0;
        writel(pr, gicd_base + GICD_IPRIORITYR + (i / 4) * 4);
        pr = readl(gicd_base + GICD_IPRIORITYR + (i / 4) * 4);
    }

    /* disable all global interrupt */
    for (i = GICV3_NR_LOCAL_IRQS; i < nr_lines; i += 32) {
        writel(0xffffffff, gicd_base + GICD_ICENABLER + (i / 32) * 4);
    }

    /* configure SPIs as non-secure GROUP-1 */
    for (i = GICV3_NR_LOCAL_IRQS; i < nr_lines; i += 32) {
        writel(0xffffffff, gicd_base + GICD_IGROUPR + (i / 32) * 4);
    }

    gicv3_gicd_wait_for_rwp(gicd_base);

    /* enable the gicd */
    writel(1 | GICD_CTLR_ENABLE_GRP1 | GICD_CTLR_ENABLE_GRP1A | GICD_CTLR_ARE_NS,
           gicd_base + GICD_CTLR);

    isb();

    w_icc_sre_el1(0xF);

    gicv3_waker_value = readl(gicr_base + GICR_WAKER);
    gicv3_waker_value &= ~(GICR_WAKER_PROCESSOR_SLEEP);
    writel(gicv3_waker_value, gicr_base + GICR_WAKER);

    while ((readl(gicr_base + GICR_WAKER) & GICR_WAKER_CHILDREN_ASLEEP) != 0)
        ;

    /* set the priority on PPI and SGI */
    pr = (0x90 << 24) | (0x90 << 16) | (0x90 << 8) | 0x90;
    for (i = 0; i < GICV3_NR_SGI; i += 4) {
        writel(pr, gicr_sgi_base + GICR_IPRIORITYR0 + (i / 4) * 4);
    }

    pr = (0xa0 << 24) | (0xa0 << 16) | (0xa0 << 8) | 0xa0;
    for (i = GICV3_NR_SGI; i < GICV3_NR_LOCAL_IRQS; i += 4) {
        writel(pr, gicr_sgi_base + GICR_IPRIORITYR0 + (i / 4) * 4);
    }

    /* disable all PPI and enable all SGI */
    writel(0xffff0000, gicr_sgi_base + GICR_ICENABLER);
    writel(0x0000ffff, gicr_sgi_base + GICR_ISENABLER);

    /* configure SGI and PPI as non-secure Group-1 */
    writel(0xffffffff, gicr_sgi_base + GICR_IGROUPR0);

    isb();

    w_icc_pmr_el1(0xff);
    w_icc_igrpen1_el1(1);
    isb();

    return 0;
}

int gicv3_driver_init(void)
{
    set_irq_chip(&gicv3_chip);
    set_handle_irq(gicv3_handle_irq);

    gicv3_hw_init((void *)0x08000000, (void *)0x080A0000, 1);

    return 0;
}
