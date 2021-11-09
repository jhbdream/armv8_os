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
    while (1)
    {
        log_v("i am %s run! line:[%d]!", __func__, __LINE__);
        mdelay(50);
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
    while (1)
    {
        log_v("i am %s run! line:[%d]!", __func__, __LINE__);
        task_sleep_ms(500);
    }
}

char taskc_stack[4096 * 8];
void taskc_fun(void)
{
    while (1)
    {
        log_v("i am %s run! line:[%d]!", __func__, __LINE__);
        task_sleep_ms(500);
    }
}

/**
 * @brief init create task and switch to taska
 *
 */
void my_task_init(void)
{
    struct task *taska_p;
    struct task *taskb_p;
    struct task *taskc_p;

    kernel_task_init();

    taska_p = task_create(taska_stack + sizeof(taska_stack), taska_fun, 10);
    taskb_p = task_create(taskb_stack + sizeof(taskb_stack), taskb_fun, 20);
    taskc_p = task_create(taskc_stack + sizeof(taskc_stack), taskc_fun, 30);

    task_switch_to(taska_p);
}