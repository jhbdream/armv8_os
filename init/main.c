#include <printk.h>
#include <driver/console.h>
#include <stdint.h>
#include <config.h>
#include <ee/irqflags.h>
#include <ee/init.h>

void start_kernel(void)
{
    console_init();
    setup_arch();

	printk("hello kernel!\n");

	while (1);
}
