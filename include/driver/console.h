#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define SECTION_CONSOLE  __attribute__((section("__console")))

struct console {
    char  name[16];
    void *arg;
    int (*init)(struct console *);
    void (*write)(struct console *, const char *, unsigned);
    void (*read)(struct console *, char *, unsigned);
};

void console_init(void);

extern struct console __console_device_begin[];
extern struct console __console_device_end[];

#define list_for_each_console(con) \
    for (con = __console_device_begin; con < __console_device_end; con++)

#endif
