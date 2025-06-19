#include <asm/memory.h>

void create_kernel_map(void);

void setup_arch(void)
{
    create_kernel_map();
}
