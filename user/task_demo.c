#include <kernel/task.h>
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
        log_v("i am taska run! line: %d", __LINE__);
        log_v("i am taska run! line: %d", __LINE__);
        mdelay(500);
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
        log_v("i am taskb run! line: %d", __LINE__);
        log_v("i am taskb run! line: %d", __LINE__);
        mdelay(500);
    }
}

/**
 * @brief init create task and switch to taska
 *
 */
void my_task_init(void)
{
    task_init(&taska, taska_stack + sizeof(taska_stack), taska_fun);
    task_init(&taskb, taskb_stack + sizeof(taskb_stack), taskb_fun);
    task_switch_to(&taska);
}