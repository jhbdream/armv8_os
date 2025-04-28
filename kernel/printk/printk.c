#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

#include <printk.h>
#include <driver/console.h>

struct console *uart_console = NULL;

int vprintk(const char *fmt, va_list args)
{
    char p_buf[1024];
    int  len;

    if (uart_console == NULL) {
        return 0;
    }

    len = vsnprintf(p_buf, sizeof(p_buf), fmt, args);

    uart_console->write(uart_console, p_buf, len);

    return len;
}

int printk(const char *fmt, ...)
{
    va_list args;
    int     r;

    va_start(args, fmt);
    r = vprintk(fmt, args);
    va_end(args);

    return r;
}
