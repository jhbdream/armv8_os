#include <kernel/task.h>
#include <stdio.h>
#include <common/delay.h>
/**
 * @brief taska
 *
 */
struct task taska;
char taska_stack[4096];
extern struct task taskb;

void taska_fun(void)
{
    while (1)
    {
        printf("i am taska run!\n");
        mdelay(1000);
        task_switch_to(&taskb);
    }
}

/**
 * @brief taskb
 *
 */
struct task taskb;
char taskb_stack[4096];

void taskb_fun(void)
{
    while (1)
    {
        printf("i am taskb run!\n");
        mdelay(1000);
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