#include <mm/memblock.h>

void create_kernel_map(void);

void setup_arch(void)
{

	create_kernel_map();

	while (1)
		;

#if 0
	memblock_add(0x40000000, 0x40000000);
	memblock_reserve(0x40000000, 0x10000000);
#endif
}
