#include <stdio.h>
#include <shell.h>

void printf_test(void)
{
    printf("parameter: [%4$d %3$d]\n", 1, 2, 3, 4);

    printf("static width: [%10d]\n", 12345);
    printf("dynamic width: [%*d]\n", 10, 12345);
    printf("dynamic width: [%*d]\n", 20, 12345);

    printf("left flag(-): [%-10d]\n", 12345);

    printf("flag+ [%+d]\n", 12345);
    printf("flag+ [%+d]\n", -12345);

    printf("flag space( ): [% d]\n", 12345);
    printf("flag space( ): [% d]\n", -12345);

    printf("Precision: [%.3f]\n", 123.45678);
    printf("Precision: [%.3s]\n", "abcdef");

}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
format, printf_test, show all printf format);