#include <printk.h>
#include <driver/console.h>
#include <stdint.h>
#include <config.h>
#include <ee/irqflags.h>

void start_kernel(void)
{
	console_init();

#ifdef CONFIG_AARCH64
	extern int arm_gicv3_interrupu_init(void *dist_base, void *rdist_base, uint32_t nr_redist_regions);
	// for qemu hardware address
	arm_gicv3_interrupu_init((void *)0x08000000, (void *)0x080A0000, 1);

	extern void aarch64_timer_init(void);
	aarch64_timer_init();
	local_irq_enable();
#endif

	printk("hello kernel!\n");

	while (1);
}
