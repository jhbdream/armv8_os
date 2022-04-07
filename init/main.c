#include <printk.h>
#include <irqflags.h>
#include <init.h>

void start_kernel(void)
{
	local_irq_disable();	
	setup_arch();


	printk("[riscv64]: hello kernel!\n");
}
