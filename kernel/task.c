#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <eeos/irqflags.h>

#include <kernel/task.h>

struct task *create_task(char *name, task_func_t func void *stack_base,
                         uint32_t stack_size, uint32_t flags)
{
    return NULL;
}
