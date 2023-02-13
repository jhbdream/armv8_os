#include <eeos.h>
#include <string.h>
#include <driver/console.h>

extern struct console *uart_console;

void console_init(void)
{
	struct console *con;

	list_for_each_console(con)
	{
		if (0 != strcmp(con->name, CONFIG_CONSOLE_NAME)) {
			continue;
		}

		if (con->init) {
			con->init(con);
		}

		uart_console = con;
		return;
	}
}
