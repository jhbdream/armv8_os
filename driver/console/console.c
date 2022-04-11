#include <driver/console.h>

extern struct console *uart_console;

void console_init(void)
{
	struct console *con;
	
	list_for_each_console(con)
	{
		uart_console = con;	
	}	

}
