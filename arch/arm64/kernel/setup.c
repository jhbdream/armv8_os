#include <asm/memory.h>
#include <mm/memblock.h>

void create_kernel_map(void);

void setup_arch(void)
{
    create_kernel_map();
}
