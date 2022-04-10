#include <stdio.h>
#include <board_init.h>
#include <config.h>
#include <kernel/task.h>
#include <mm/simple_mm.h>
#include <printk.h>
#include <shell_port.h>

extern void sem_task_init(void);

int main()
{
    printk("start run in main!\n");
    mm_init(0x50000000, 0x10000000);

    interrupt_init();
    systic_timer_init();
    kernel_task_init();
    lettel_shell_init();
    sem_task_init(main);
    user_task_init();

    /* will not run here! */
    printk("will not run here!!!\n");
    return 0;
}
