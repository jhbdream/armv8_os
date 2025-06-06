#include <string.h>
#include <stddef.h>

#include <driver/console.h>

struct console *uart_console = NULL;

void console_init(void)
{
    struct console *con;

    list_for_each_console(con)
    {
        if (0 != strcmp(con->name, "qemu")) {
            continue;
        }

        if (con->init) {
            con->init(con);
        }

        uart_console = con;

        return;
    }
}

void console_write(const char *s, int n)
{
    if (uart_console == NULL) {
        return;
    }

    if (s == NULL) {
        return;
    }

    if (n == 0) {
        return;
    }

    uart_console->write(uart_console, s, n);
}
