#include <stdint.h>
#include <stddef.h>
#include <ee_errno.h>
#include <io.h>
#include <type.h>
#include <common/interrupt.h>
#include <arch_gicv3.h>
#include <cputype.h>
#include "arm-gic-v3.h"
#include "../irq-gic-common.h"
#include <printk.h>

struct redist_region
{
	void __iomem *redist_base;
};

struct gic_chip_data
{
	void __iomem *dist_base;
	struct redist_region redist_regions[NR_CPUS];
	u32 nr_redist_regions;
	unsigned int irq_nr;
};

static struct gic_chip_data gic_data;

/* Our default, arbitrary priority value. Linux only uses one anyway. */
#define DEFAULT_PMR_VALUE 0xf0

#define cpu_relax()
#define pr_err_ratelimited printk

static inline void __iomem *gic_data_rdist_rd_base(void)
{
	unsigned long mpidr = read_cpuid_mpidr() & MPIDR_HWID_BITMASK;
	u64 typer;
	u32 aff;
	void *ptr;
	/*
	 * Convert affinity to a 32bit value that can be matched to
	 * GICR_TYPER bits [63:32].
	 */
	aff = (MPIDR_AFFINITY_LEVEL(mpidr, 3) << 24 |
		   MPIDR_AFFINITY_LEVEL(mpidr, 2) << 16 |
		   MPIDR_AFFINITY_LEVEL(mpidr, 1) << 8 |
		   MPIDR_AFFINITY_LEVEL(mpidr, 0));

	for (int i = 0; i < gic_data.nr_redist_regions; i++)
	{
		ptr = gic_data.redist_regions[i].redist_base;
		typer = gic_read_typer(ptr + GICR_TYPER);

		if ((typer >> 32) == aff)
		{
			return ptr;
		}
	}

	return NULL;
}

static inline void __iomem *gic_data_rdist_sgi_base(void)
{
	return (gic_data_rdist_rd_base() + 0x10000);
}

static inline int gic_irq_in_rdist(u32 hwirq)
{
	return (hwirq < 32);
}

static void gic_do_wait_for_rwp(void __iomem *base)
{
	u32 count = 1000000; /* 1s! */

	while (readl_relaxed(base + GICD_CTLR) & GICD_CTLR_RWP)
	{
		count--;
		if (!count)
		{
			pr_err_ratelimited("RWP timeout, gone fishing");
			return;
		}
		cpu_relax();
	};
}

/* Wait for completion of a distributor change */
static void gic_dist_wait_for_rwp(void)
{
	gic_do_wait_for_rwp(gic_data.dist_base);
}

/* Wait for completion of a redistributor change */
static void gic_redist_wait_for_rwp(void)
{
	gic_do_wait_for_rwp(gic_data_rdist_rd_base());
}

u64 gic_read_iar(void)
{
	return gic_read_iar_common();
}

static void gic_enable_redist(bool enable)
{
	void __iomem *rbase;
	u32 count = 1000000; /* 1s! */
	u32 val;

	rbase = gic_data_rdist_rd_base();

	val = readl_relaxed(rbase + GICR_WAKER);
	if (enable)
		/* Wake up this CPU redistributor */
		val &= ~GICR_WAKER_ProcessorSleep;
	else
		val |= GICR_WAKER_ProcessorSleep;
	writel_relaxed(val, rbase + GICR_WAKER);

	if (!enable)
	{ /* Check that GICR_WAKER is writeable */
		val = readl_relaxed(rbase + GICR_WAKER);
		if (!(val & GICR_WAKER_ProcessorSleep))
			return; /* No PM support in this redistributor */
	}

	while (--count)
	{
		val = readl_relaxed(rbase + GICR_WAKER);
		if (enable ^ (bool)(val & GICR_WAKER_ChildrenAsleep))
			break;
		cpu_relax();
	};
	if (!count)
		pr_err_ratelimited("redistributor failed to %s...\n",
						   enable ? "wakeup" : "sleep");
}

/*
 * Routines to disable, enable, EOI and route interrupts
 */
static int gic_peek_irq(u32 hwirq, u32 offset)
{
	u32 mask = 1 << (hwirq % 32);
	void __iomem *base;

	if (gic_irq_in_rdist(hwirq))
		base = gic_data_rdist_sgi_base();
	else
		base = gic_data.dist_base;

	return !!(readl_relaxed(base + offset + (hwirq / 32) * 4) & mask);
}

static void gic_poke_irq(u32 hwirq, u32 offset)
{
	u32 mask = 1 << (hwirq % 32);
	void (*rwp_wait)(void);
	void __iomem *base;

	if (gic_irq_in_rdist(hwirq))
	{
		base = gic_data_rdist_sgi_base();
		rwp_wait = gic_redist_wait_for_rwp;
	}
	else
	{
		base = gic_data.dist_base;
		rwp_wait = gic_dist_wait_for_rwp;
	}

	writel_relaxed(mask, base + offset + (hwirq / 32) * 4);
	rwp_wait();
}

void gic_mask_irq(u32 hwirq)
{
	gic_poke_irq(hwirq, GICD_ICENABLER);
}

static void gic_eoimode1_mask_irq(u32 hwirq)
{
	gic_mask_irq(hwirq);
}

void gic_unmask_irq(u32 hwirq)
{
	gic_poke_irq(hwirq, GICD_ISENABLER);
}

static int gic_irq_set_irqchip_state(u32 hwirq,
									 enum irqchip_irq_state which, u8 val)
{
	u32 reg;

	switch (which)
	{
	case IRQCHIP_STATE_PENDING:
		reg = val ? GICD_ISPENDR : GICD_ICPENDR;
		break;

	case IRQCHIP_STATE_ACTIVE:
		reg = val ? GICD_ISACTIVER : GICD_ICACTIVER;
		break;

	case IRQCHIP_STATE_MASKED:
		reg = val ? GICD_ICENABLER : GICD_ISENABLER;
		break;

	default:
		return -EINVAL;
	}

	gic_poke_irq(hwirq, reg);
	return 0;
}

int gic_irq_get_irqchip_state(u32 hwirq,
								enum irqchip_irq_state which, bool *val)
{
	switch (which)
	{
	case IRQCHIP_STATE_PENDING:
		*val = gic_peek_irq(hwirq, GICD_ISPENDR);
		break;

	case IRQCHIP_STATE_ACTIVE:
		*val = gic_peek_irq(hwirq, GICD_ISACTIVER);
		break;

	case IRQCHIP_STATE_MASKED:
		*val = !gic_peek_irq(hwirq, GICD_ISENABLER);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

void gic_eoi_irq(u32 hwirq)
{
	gic_write_eoir(hwirq);
}

void gic_eoimode1_eoi_irq(u32 hwirq)
{
	gic_write_dir(hwirq);
}

static int gic_set_type(u32 hwirq, unsigned int type)
{
	unsigned int irq = hwirq;
	void (*rwp_wait)(void);
	void __iomem *base;

	/* Interrupt configuration for SGIs can't be changed */
	if (irq < 16)
		return -EINVAL;

	/* SPIs have restrictions on the supported types */
	if (irq >= 32 && type != IRQ_TYPE_LEVEL_HIGH &&
		type != IRQ_TYPE_EDGE_RISING)
		return -EINVAL;

	if (gic_irq_in_rdist(hwirq))
	{
		base = gic_data_rdist_sgi_base();
		rwp_wait = gic_redist_wait_for_rwp;
	}
	else
	{
		base = gic_data.dist_base;
		rwp_wait = gic_dist_wait_for_rwp;
	}

	return gic_configure_irq(irq, type, base, rwp_wait);
}

static u64 gic_mpidr_to_affinity(unsigned long mpidr)
{
	u64 aff;

	aff = ((u64)MPIDR_AFFINITY_LEVEL(mpidr, 3) << 32 |
		   MPIDR_AFFINITY_LEVEL(mpidr, 2) << 16 |
		   MPIDR_AFFINITY_LEVEL(mpidr, 1) << 8 |
		   MPIDR_AFFINITY_LEVEL(mpidr, 0));

	return aff;
}

static int gic_validate_dist_version(void __iomem *dist_base)
{
	u32 reg = readl_relaxed(dist_base + GICD_PIDR2) & GIC_PIDR2_ARCH_MASK;

	if (reg != GIC_PIDR2_ARCH_GICv3 && reg != GIC_PIDR2_ARCH_GICv4)
		return -ENODEV;

	return 0;
}

static void gic_dist_init(void)
{
	unsigned int i;
	u64 affinity;
	void __iomem *base = gic_data.dist_base;

	/* Disable the distributor */
	writel_relaxed(0, base + GICD_CTLR);
	gic_dist_wait_for_rwp();

	/*
	 * Configure SPIs as non-secure Group-1. This will only matter
	 * if the GIC only has a single security state. This will not
	 * do the right thing if the kernel is running in secure mode,
	 * but that's not the intended use case anyway.
	 */
	for (i = 32; i < gic_data.irq_nr; i += 32)
		writel_relaxed(~0, base + GICD_IGROUPR + i / 8);

	gic_dist_config(base, gic_data.irq_nr, gic_dist_wait_for_rwp);

	/* Enable distributor with ARE, Group1 */
	writel_relaxed(GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1A | GICD_CTLR_ENABLE_G1,
				   base + GICD_CTLR);

	/*
	 * Set all global interrupts to the boot CPU only. ARE must be
	 * enabled.
	 */
	u32 mpidr = read_cpuid_mpidr() & MPIDR_HWID_BITMASK;
	affinity = gic_mpidr_to_affinity(mpidr);

	for (i = 32; i < gic_data.irq_nr; i++)
		gic_write_irouter(affinity, base + GICD_IROUTER + i * 8);
}

static void gic_cpu_sys_reg_init(void)
{
	int i;
	bool group0;
	u32 val, pribits;

	pribits = gic_read_ctlr();
	pribits &= ICC_CTLR_EL1_PRI_BITS_MASK;
	pribits >>= ICC_CTLR_EL1_PRI_BITS_SHIFT;
	pribits++;

	/*
	 * Let's find out if Group0 is under control of EL3 or not by
	 * setting the highest possible, non-zero priority in PMR.
	 *
	 * If SCR_EL3.FIQ is set, the priority gets shifted down in
	 * order for the CPU interface to set bit 7, and keep the
	 * actual priority in the non-secure range. In the process, it
	 * looses the least significant bit and the actual priority
	 * becomes 0x80. Reading it back returns 0, indicating that
	 * we're don't have access to Group0.
	 */
	write_gicreg(BIT(8 - pribits), ICC_PMR_EL1);
	val = read_gicreg(ICC_PMR_EL1);
	group0 = val != 0;

	/* Set priority mask register */
	write_gicreg(DEFAULT_PMR_VALUE, ICC_PMR_EL1);

	/*
	 * Some firmwares hand over to the kernel with the BPR changed from
	 * its reset value (and with a value large enough to prevent
	 * any pre-emptive interrupts from working at all). Writing a zero
	 * to BPR restores is reset value.
	 */
	gic_write_bpr1(0);

	gic_write_ctlr(ICC_CTLR_EL1_EOImode_drop_dir);

	/* Always whack Group0 before Group1 */
	if (group0)
	{
		switch (pribits)
		{
		case 8:
		case 7:
			write_gicreg(0, ICC_AP0R3_EL1);
			write_gicreg(0, ICC_AP0R2_EL1);
		case 6:
			write_gicreg(0, ICC_AP0R1_EL1);
		case 5:
		case 4:
			write_gicreg(0, ICC_AP0R0_EL1);
		}

		isb();
	}

	switch (pribits)
	{
	case 8:
	case 7:
		write_gicreg(0, ICC_AP1R3_EL1);
		write_gicreg(0, ICC_AP1R2_EL1);
	case 6:
		write_gicreg(0, ICC_AP1R1_EL1);
	case 5:
	case 4:
		write_gicreg(0, ICC_AP1R0_EL1);
	}

	isb();

	/* ... and let's hit the road... */
	gic_write_grpen1(1);
}

static void gic_cpu_init(void)
{
	void __iomem *rbase;

	gic_enable_redist(true);

	rbase = gic_data_rdist_sgi_base();

	/* Configure SGIs/PPIs as non-secure Group-1 */
	writel_relaxed(~0, rbase + GICR_IGROUPR0);

	gic_cpu_config(rbase, gic_redist_wait_for_rwp);

	/* initialise system registers */
	gic_cpu_sys_reg_init();
}

int gic_init_bases(void __iomem *dist_base,
				   void __iomem *rdist_base,
				   u32 nr_redist_regions)
{
	u32 typer;
	int gic_irqs;
	int i, err;
	struct redist_region *rdist_regs;

	err = gic_validate_dist_version(dist_base);
	if (err) {
		return err;
	}

	gic_data.nr_redist_regions = nr_redist_regions;
	gic_data.dist_base = dist_base;

	for (i = 0; i < nr_redist_regions; i++)
	{
		gic_data.redist_regions[i].redist_base = rdist_base;
		rdist_base = rdist_base + 0x20000;
	}

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources (SGI+PPI+SPI)
	 */
	typer = readl_relaxed(gic_data.dist_base + GICD_TYPER);
	gic_irqs = GICD_TYPER_IRQS(typer);
	if (gic_irqs > 1020)
		gic_irqs = 1020;

	gic_data.irq_nr = gic_irqs;

	gic_dist_init();
	gic_cpu_init();

	return 0;
}
