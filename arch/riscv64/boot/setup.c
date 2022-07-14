#include <ee/irqflags.h>

void setup_arch(void)
{
	extern void riscv_plic_init(void);
	riscv_plic_init();
	local_irq_enable();
}
