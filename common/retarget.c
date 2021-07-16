#include <stdint.h>

void _exit(int return_value)
{
    asm ("dsb sy");
    while (1)
    {
        asm ("wfi");
    }
}