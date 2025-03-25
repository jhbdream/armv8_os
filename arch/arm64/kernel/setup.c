#include <mm/memblock.h>

void setup_arch(void)
{

	void init_pagetables(void);

	init_pagetables();

#if 0
	memblock_add(0x40000000, 0x40000000);
	memblock_reserve(0x40000000, 0x10000000);
#endif
}
