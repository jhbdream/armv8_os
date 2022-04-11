#include <printk.h>
#include <driver/console.h>

void start_kernel(void)
{
	console_init();
	printk("hello kernel!\n");
}
