#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include <driver/uart.h>

__attribute__((weak)) int uart_putchar(uint8_t ch);
__attribute__((weak)) int uart_getchar(uint8_t *ch);

void initialise_monitor_handles(void)
{
    /* 关闭行缓冲 */
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
}

int _read(int file, char *ptr, int len)
{
    int i;
    uint8_t ch;
    int read_len = 0;

    for (i = 0; i < len; ++i)
    {
        if(uart_getchar(&ch) == 0 )
        {
            ptr[read_len] = ch;
            read_len++;
        }
    }
    return read_len;
}

int _write(int file, char *ptr, int len)
{
    int i;
    int write_len = 0;
    for (i = 0; i < len; ++i)
    {
        if (ptr[i] == '\n')
        {
           while (uart_putchar('\r') < 0);
        }

        if(uart_putchar(ptr[write_len]) == 0)
        {
            write_len++;
        }
    }

    return write_len;
}

int _lseek(int file, int ptr, int dir)
{
    errno = ESPIPE;
    return -1;
}

int _close(int file)
{
    return -1;
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd)
{
    errno = ENOTTY;
    return 0;
}

void* _sbrk(int increment)
{
    extern char end; // From linker script
    static char* heap_end = &end;

    char* current_heap_end = heap_end;
    heap_end += increment;
    return current_heap_end;
}

void _exit(int return_value)
{
    asm ("dsb sy");
    while (1)
    {
        asm ("wfi");
    }
}

int _getpid(void)
{
  return 1;
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}