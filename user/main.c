#include <stdio.h>
#include <elog.h>
#include <board_init.h>

int main()
{
    printf("hello world\n");
    printf("I am armv8 os!\n");

    my_elog_init();

    return 0;
}
