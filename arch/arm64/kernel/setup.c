#include <mm/memblock.h>

void setup_arch(void)
{
	memblock_add(0x40000000, 0x40000000);
	memblock_reserve(0x40000000, 0x10000000);
}
