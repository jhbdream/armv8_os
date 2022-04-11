#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include <stringify.h>
#include <compiler_types.h>

struct console
{
    char name[16];
    void (*write)(struct console *, const char *, unsigned);
    void (*read)(struct console *, char *, unsigned);
};

void console_init(void);

#define CONSOLE_DECLARE(_name, _write, _read)  \
    static const struct console __UNIQUE_ID(__console_##_name)    \
    __attribute__((used)) __attribute__((__section__("__console"))) \
    {                                               \
        .name = __stringify(_name);                 \
        .write = _write                             \
        .read = _read                               \
    };                                              \

#endif