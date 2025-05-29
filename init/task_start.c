#include <kernel/task.h>
#include <kernel/tick.h>
#include <common/delay.h>
#include <stddef.h>
#include <printk.h>
#include <string.h>

/**
 * @brief taska
 *
 */
struct task taska;
struct task taskb;

char task_idle_stack[4096];
char taska_stack[4096];
char taskb_stack[4096];

static void task_idle(void)
{
    while (1) {
    }
}

void taska_fun(void)
{
    static uint64_t taska_count = 0;
    while (1) {
        task_sleep_ms(1000);
        printk("taska: [%d]\n", taska_count++);
    }
}

void taskb_fun(void)
{
    static uint64_t taskb_count = 0;
    while (1) {
        task_sleep_ms(500);
        printk("taskb: [%d]\n", taskb_count++);
    }
}

/**
 * @brief init create task and switch to taska
 *
 */
void user_task_init(void)
{
    struct task *taskp;

    memset(taska_stack, 0x5a, sizeof(taska_stack));
    task_create("taska", taska_stack + sizeof(taska_stack), taska_fun, 20);

    memset(taskb_stack, 0x5a, sizeof(taskb_stack));
    task_create("taskb", taskb_stack + sizeof(taskb_stack), taskb_fun, 21);

    memset(task_idle_stack, 0x5a, sizeof(task_idle_stack));
    taskp = task_create("idle", task_idle_stack + sizeof(task_idle_stack), task_idle, 0);
    task_switch_to(taskp);
}
