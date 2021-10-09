#include <stdio.h>
#include <elog.h>
#include <board_init.h>
#include <config.h>

extern void my_task_init(void);

int main()
{
#if 0
    my_elog_init();

    interrupt_init();
    arch_timer_test();
#endif

    printf("run in main!\n");

    my_task_init();

    while (1)
    {
        /* code */
    }

    return 0;
}
