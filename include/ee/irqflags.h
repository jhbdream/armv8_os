#ifndef _EE_IRQFLAGS_H_
#define _EE_IRQFLAGS_H_

#include <asm/irqflags.h>

#define local_irq_enable()                                                     \
	do {                                                                   \
		arch_local_irq_enable();                                       \
	} while (0)

#define local_irq_disable()                                                    \
	do {                                                                   \
		arch_local_irq_disable();                                      \
	} while (0)

#define local_irq_save(flags)                                                  \
	do {                                                                   \
		flags = arch_local_irq_save();                                 \
	} while (0)

#define local_irq_restore(flags)                                               \
	do {                                                                   \
		arch_local_irq_restore(flags);                                 \
	} while (0)

#endif
