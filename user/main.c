#include <stdio.h>
#include <elog.h>
#include <board_init.h>
#include <config.h>

extern void my_task_init(void);
extern void timer_init(void);

int main()
{
    printf("run in main!\n");
    interrupt_init();
    my_elog_init();
    timer_init();
    my_task_init();

    while (1)
    {
        /* code */
    }

    return 0;
}
