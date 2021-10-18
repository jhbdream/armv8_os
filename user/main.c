#include <stdio.h>
#include <elog.h>
#include <board_init.h>
#include <config.h>

extern void my_task_init(void);

int main()
{
   printf("run in main!\n");

    my_elog_init();
    arch_timer_test();

    my_task_init();

    while (1)
    {
        /* code */
    }

    return 0;
}
