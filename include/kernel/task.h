#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

#define TASK_NAME_SIZE (32)

typedef enum _task_state {
    TASK_STATE_FREE,
    TASK_STATE_INIT,
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_WAIT,
} task_state;

struct task {
    void *stack_base;
    void *stack_top;
    void *stack_bottom;

    task_state state;

    char name[TASK_NAME_SIZE];
};

typedef int (*task_func_t)(void *data);

struct task *create_task(char *name, task_func_t func, void *stack_base,
                         uint32_t stack_size, uint32_t flags);

#endif // !__TASK_H__
