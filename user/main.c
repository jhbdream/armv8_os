#include <stdio.h>
#include <elog.h>
#include <board_init.h>
#include <config.h>

int main()
{
    my_elog_init();

    interrupt_init();
    arch_timer_test();

    while (1)
    {
        /* code */
    }

    return 0;
}
