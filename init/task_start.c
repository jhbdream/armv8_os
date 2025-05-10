#include <kernel/task.h>
#include <kernel/tick.h>
#include <common/delay.h>
#include <stddef.h>
#include <printk.h>

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
    static uint64_t idel_count = 0;

    while (1) {
        idel_count++;
        printk("task idle: [%d]\n", idel_count);
    }
}

void taska_fun(void)
{
    while (1) {
        //    task_sleep_ms(1000);
        printk("taska\n");
    }
}

void taskb_fun(void)
{
    while (1) {
        task_sleep_ms(500);
        // printk("taskb\n");
    }
}

/**
 * @brief init create task and switch to taska
 *
 */
void user_task_init(void)
{
    struct task *taskp;

#if 0
    task_create("taskb", taskb_stack + sizeof(taskb_stack), taskb_fun, 21);
#endif

    task_create("taska", taska_stack + sizeof(taska_stack), taska_fun, 20);

    taskp = task_create("idle", task_idle_stack + sizeof(task_idle_stack), task_idle, 0);
    task_switch_to(taskp);
}
