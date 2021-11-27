#include <shell.h>
#include <driver/uart.h>
#include <kernel/task.h>

#include <elog.h>


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
    int i;
    uint8_t ch;
    int read_len = 0;

    for (i = 0; i < len; ++i)
    {
        if(uart_getchar(&ch) == 0 )
        {
            data[read_len] = ch;
            read_len++;
        }
    }
    return read_len;
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
    int i;
    int write_len = 0;
    for (i = 0; i < len; ++i)
    {
        if(uart_putchar(data[write_len]) == 0)
        {
            write_len++;
        }
    }

    return write_len;
}


/**
 * @brief shell 任务
 *
 * @param param 参数(shell对象)
 *
 */
void shellTask(void)
{
    Shell *shell_handle = (Shell *)&shell;
    char data;
#if SHELL_TASK_WHILE == 1
    while(1)
    {
#endif
        if (shell_handle->read && shell_handle->read(&data, 1) == 1)
        {
            shellHandler(shell_handle, data);
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

    task_create("lettel_shell", shell_task_stack + sizeof(shell_task_stack), shellTask, 10);
}