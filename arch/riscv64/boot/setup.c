#include "driver/console.h"
#include <ee/irqflags.h>

void setup_arch(void)
{
	extern void riscv_plic_init(void);
	riscv_plic_init();
	local_irq_enable();

	console_init();

	extern void riscv_timer_init(void);
	riscv_timer_init();
}
