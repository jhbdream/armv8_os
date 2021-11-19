#include <shell.h>
#include <driver/uart.h>
#include <kernel/task.h>

Shell shell;
char shellBuffer[512];

/**
 * @brief 通过串口读取len字节数据
 *
 * @param data
 * @param len
 * @return short
 */
short shell_port_read(char *data, unsigned short len)
{
    short i;

    for(i = 0; i < len; i++)
    {
        data[i] = uart_getchar_polled();
    }

    return i;
}

/**
 * @brief 通过串口发送len长度数据
 *
 * @param data
 * @param len
 * @return short
 */
short shell_port_write(char *data, unsigned short len)
{
    short i;

    for(i = 0; i < len; i++)
    {
        uart_putc_polled(data[i]);
    }

    return i;
}


/**
 * @brief shell 任务
 *
 * @param param 参数(shell对象)
 *
 */
void shellTask(void)
{
    Shell *shell = (Shell *)shell;
    char data;
#if SHELL_TASK_WHILE == 1
    while(1)
    {
#endif
        if (shell->read && shell->read(&data, 1) == 1)
        {
            shellHandler(shell, data);
        }
#if SHELL_TASK_WHILE == 1
    }
#endif
}

char shell_task_stack[4096 * 8];

void lettel_shell_init(void)
{
    shell.read = shell_port_read;
    shell.write = shell_port_write;
    shellInit(&shell, shellBuffer, sizeof(shellBuffer));

    task_create(shell_task_stack + sizeof(shell_task_stack), shellTask, 10);
}