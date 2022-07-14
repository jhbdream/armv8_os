#include <ee/irqflags.h>

void setup_arch(void)
{
	extern void riscv_plic_init(void);
	riscv_plic_init();
	local_irq_enable();

	extern void riscv_timer_start(void);
	riscv_timer_start();
}
