#include <kernel/task.h>
#include <kernel/tick.h>

#include <stdio.h>
#include <common/delay.h>
#include <elog.h>

extern struct task taska;
extern struct task taskb;


/**
 * @brief taska
 *
 */
struct task taska;
char taska_stack[4096 * 8];

void taska_fun(void)
{
    log_i("task pid is %d!", getpid());
    log_i("i am %s run! line:[%d]!", __func__, __LINE__);

    while (1)
    {
        task_sleep_ms(50);
    }
}

/**
 * @brief taskb
 *
 */
struct task taskb;
char taskb_stack[4096 * 8];

void taskb_fun(void)
{
    log_i("task pid is %d!", getpid());
    log_i("i am %s run! line:[%d]!", __func__, __LINE__);
    while (1)
    {
        task_sleep_ms(500);
    }
}

char task_idle_stack[4096];
void task_idle(void)
{
    log_i("task pid is %d!", getpid());
    log_i("i am %s run! line:[%d]!", __func__, __LINE__);

    while (1)
    {

    }
}

/**
 * @brief init create task and switch to taska
 *
 */
void user_task_init(void)
{
    struct task *taskp;

    taskp = task_create("taskA", taska_stack + sizeof(taska_stack), taska_fun, 20);
    task_create("taskB", taskb_stack + sizeof(taskb_stack), taskb_fun, 21);
    task_create("idle", task_idle_stack + sizeof(task_idle_stack), task_idle, 0);

    task_switch_to(taskp);
}