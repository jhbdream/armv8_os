#include <printk.h>
#include <shell.h>

void printf_test(void)
{
	printk("hex: [%#x]\n", 0x1234);
	printk("hex: [0x%x]\n", 0x1234);

	printk("static width: [%10d]\n", 12345);
	printk("dynamic width: [%*d]\n", 10, 12345);
	printk("dynamic width: [%*d]\n", 20, 12345);

	printk("left flag(-): [%-10d]\n", 12345);

	printk("flag+ [%+d]\n", 12345);
	printk("flag+ [%+d]\n", -12345);

	printk("flag space( ): [% d]\n", 12345);
	printk("flag space( ): [% d]\n", -12345);

	//printk("Precision: [%.3f]\n", 123.45678);
	printk("Precision: [%.3s]\n", "abcdef");
	printk("str: [%s]\n", "abcdef");
	printk("char: [%c]\n", 'a');
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) |
			 SHELL_CMD_DISABLE_RETURN,
		 format, printf_test, show all printk format);