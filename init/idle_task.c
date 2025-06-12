#include <stddef.h>
#include <string.h>

#include <kernel/task.h>

static uint64_t idle_cnount = 0;
static char     task_idle_stack[4096];

static void task_idle(void)
{
    while (1) {
        idle_cnount++;
    }
}

/**
 * @brief init create task and switch to taska
 *
 */
void idle_task_init(void)
{
    struct task *taskp;

    memset(task_idle_stack, 0x5a, sizeof(task_idle_stack));

    taskp = task_create("idle", task_idle_stack + sizeof(task_idle_stack), task_idle, 0);
    task_switch_to(taskp);
}
