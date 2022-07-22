#include "driver/console.h"
#include "printk.h"
#include <ee/irqflags.h>
#include <type.h>

void setup_arch(void)
{
	extern void rv64_memblock_init(phys_addr_t start, phys_addr_t end);
	rv64_memblock_init(0x80000000, 0xc0000000);

	extern void setup_vm_final(void);
	setup_vm_final();

#if 0
	extern void riscv_plic_init(void);
	riscv_plic_init();
	local_irq_enable();
#endif

	console_init();

	printk("hello world\n");

	while(1);

	extern void riscv_timer_init(void);
	riscv_timer_init();
}
