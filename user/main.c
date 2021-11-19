#include <stdio.h>
#include <elog.h>
#include <board_init.h>
#include <config.h>
#include <shell_port.h>
#include <kernel/task.h>

int main()
{
    printf("start run in main!\n");
    elog_lib_init();

    interrupt_init();
    systic_timer_init();
    kernel_task_init();

    lettel_shell_init();
    user_task_init();

    /* will not run here! */
    printf("will not run here!!!\n");
    return 0;
}
