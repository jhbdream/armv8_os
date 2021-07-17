#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include <driver/uart.h>

__attribute__((weak)) uint8_t uart_getchar_polled(void);
__attribute__((weak)) void uart_putc_polled(uint8_t c);

void initialise_monitor_handles(void)
{
    /* 关闭行缓冲 */
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
}

int _read(int file, char *ptr, int len)
{
    int i;

    for (i = 0; i < len; ++i)
    {
        ptr[i] = uart_getchar_polled();
    }
    return len;
}

int _write(int file, char *ptr, int len)
{
    int i;
    for (i = 0; i < len; ++i)
    {
        if (ptr[i] == '\n')
        {
            uart_putc_polled('\r');
        }

        uart_putc_polled(ptr[i]);
    }

    return len;
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