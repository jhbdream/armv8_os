#include <kernel/task.h>
#include <stdio.h>

/**
 * @brief taska
 *
 */
struct task taska;
char taska_stack[4096];

void taska_fun(void)
{
    while (1)
    {
        printf("i am taska run!\n");
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
    }
}

void my_task_init(void)
{
    task_init(&taska, taska_stack + sizeof(taska_stack), taska_fun);
    task_init(&taskb, taskb_stack + sizeof(taskb_stack), taskb_fun);
    task_switch_to(&taska);
}