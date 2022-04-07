#include <stdarg.h>
#include <stdio.h>
#include <printk.h>

extern int _write(int file, char *ptr, int len);

int vprintk(const char *fmt, va_list args)
{
	char p_buf[1024];
	int len;
	len = vsnprintf(p_buf, sizeof(p_buf), fmt, args);
	_write(0, p_buf, len);
	return len;
}

int printk(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}
