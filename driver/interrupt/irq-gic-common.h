#ifndef __IRQ_GIC_COMMON_H__
#define __IRQ_GIC_COMMON_H__

#include <io.h>

int gic_configure_irq(unsigned int irq, unsigned int type, void *base,
		      void (*sync_access)(void));
void gic_dist_config(void *base, int gic_irqs,
		     void (*sync_access)(void));
void gic_cpu_config(void *base, void (*sync_access)(void));

#endif /* __IRQ_GIC_COMMON_H__ */
