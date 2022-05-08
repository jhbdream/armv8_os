#include <mm/memblock.h>
#include <stdint.h>

#define INIT_MEMBLOCK_REGIONS           128
#define INIT_PHYSMEM_REGIONS            4
#define INIT_MEMBLOCK_RESERVED_REGIONS     INIT_MEMBLOCK_REGIONS

static struct memblock_region memblock_memory_init_regions[INIT_MEMBLOCK_REGIONS];
static struct memblock_region memblock_reserved_init_regions[INIT_MEMBLOCK_RESERVED_REGIONS];

// 静态初始化 memblock
struct memblock memblock =
{
    .memory.regions = memblock_memory_init_regions,
    .memory.cnt = 1,
    .memory.max = INIT_MEMBLOCK_REGIONS,
    .memory.name = "memory",

    .reserved.regions = memblock_reserved_init_regions,
    .reserved.cnt = 1,
    .reserved.max = INIT_MEMBLOCK_RESERVED_REGIONS,
    .reserved.name = "reserved",
};

static struct memblock_type *memblock_memory = &memblock.memory;

#define for_each_memblock_type(i, memblock_type, rgn)           \
    for (i = 0, rgn = &memblock_type->regions[0];           \
         i < memblock_type->cnt;                    \
         i++, rgn = &memblock_type->regions[i])

static int memblock_add_range(struct memblock_type *type, phys_addr_t base, phys_addr_t size)
{
    bool insert = false;
    phys_addr_t obase = base;
    phys_addr_t end = base + size;
    int idx, nr_new;
    struct memblock_region *rgn;

    if(!size)
    {
        return 0;
    }

    // first add range
    if(type->regions[0].size == 0)
    {
        type->regions[0].base = base;
        type->regions[0].size = size;
        type->total_size = size;
        return 0;
    }

    for_each_memblock_type(idx, type, rgn)
    {
        phys_addr_t rbase = rgn->base;
        phys_addr_t rend = rgn->base + rgn->size;

        if(rbase >= end)
        {
            break;
        }
        if(rend <= base)
        {
            continue;
        }

    }
}

int memblock_add(phys_addr_t base, phys_addr_t size)
{

}

static int memblock_remove_range(struct memblock_type *type, phys_addr_t base, phys_addr_t size)
{

}

int memblock_remove(phys_addr_t base, phys_addr_t size)
{

}

static void *memblock_alloc(phys_addr_t size, phys_addr_t align)
{

}

void memblock_free(void *ptr, size_t size)
{

}