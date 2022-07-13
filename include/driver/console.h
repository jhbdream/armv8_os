#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include <stringify.h>
#include <compiler_types.h>

struct console
{
    char name[16];
	void *arg;
    int (*init)(struct console *);
    void (*write)(struct console *, const char *, unsigned);
    void (*read)(struct console *, char *, unsigned);
};

void console_init(void);

/**
 * 定义一个控制台输出信息
 *
 * _name 名称
 * _arg 传递一个参数
 * _write 输出接口
 * _read 输入接口
 */
#define CONSOLE_DECLARE(_name, _arg, _init, _write, _read)  \
    static const struct console __UNIQUE_ID(_console_##_name)    \
    __attribute__((__used__)) __attribute__((__section__("__console"))) = \
    {                                               \
        .name = __stringify(_name),                 \
		.arg = _arg,								\
        .init = _init,                              \
        .write = _write,			                \
        .read = _read,                              \
    };                                              \

extern struct console __console_device_begin[];
extern struct console __console_device_end[];

#define list_for_each_console(con) \
	for (con = __console_device_begin; con < __console_device_end; con++)                                       

#endif
