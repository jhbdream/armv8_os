#ifndef __TASK_H__
#define __TASK_H__

#include "libc/stdint.h"
#include <stdint.h>

#define TASK_NAME_SIZE (32)

struct task {
    void *stack_base;
    void *stack_top;
    void *stack_bottom;

    int tid;

    uint32_t flag;
    uint32_t state;

    char name[TASK_NAME_SIZE];
};

typedef int (*task_func_t)(void *data);

struct task *create_task(char *name, task_func_t func void *stack_base,
                         uint32_t stack_size, uint32_t flags);

#endif // !__TASK_H__
