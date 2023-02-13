#ifndef __IRQ_GIC_COMMON_H__
#define __IRQ_GIC_COMMON_H__

#include <io.h>

int gic_configure_irq(unsigned int irq, unsigned int type, void __iomem *base,
		      void (*sync_access)(void));
void gic_dist_config(void __iomem *base, int gic_irqs,
		     void (*sync_access)(void));
void gic_cpu_config(void __iomem *base, void (*sync_access)(void));

#endif /* __IRQ_GIC_COMMON_H__ */
