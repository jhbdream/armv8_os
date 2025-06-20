#ifndef __EE_STDIO_H__
#define __EE_STDIO_H__

#include <stdarg.h>
#include <stdint.h>

int vsprintf(char *buf, const char *fmt, va_list args);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int snprintf(char *buf, size_t size, const char *fmt, ...);

#endif