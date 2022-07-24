/**
 * @file memblock.c
 * @author jihongbin (longma@orbbec.com)
 * @brief
 * @version 0.1
 * @date 2022-05-09
 * @copyright Copyright (c) {2015-2021} Orbbec
 *
 */

#include "type.h"
#include <mm/memblock.h>
#include <ee/minmax.h>
#include <ee/errno.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <printk.h>
#include <limits.h>
#include <asm/memory.h>
#include <ee/pfn.h>
#include <mm/page_alloc.h>
#include <bitops.h>

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

#define memblock_dbg(fmt, ...)                      \
    do {                                \
        if (memblock_debug)                 \
            printk(fmt, ##__VA_ARGS__);            \
    } while (0)

static int memblock_debug;

static void memblock_insert_region(struct memblock_type *type, int idx, phys_addr_t base, phys_addr_t size)
{
    struct memblock_region *rgn = &type->regions[idx];

    memmove(rgn + 1, rgn, (type->cnt - idx) * sizeof(*rgn));

    rgn->base = base;
    rgn->size = size;

    type->cnt++;
    type->total_size += size;
}

static void memblock_remove_region(struct memblock_type *type, int r)
{
    type->total_size -= type->regions[r].size;
    memmove(&type->regions[r], &type->regions[r + 1], (type->cnt - (r + 1)) * sizeof(type->regions[r]));
    type->cnt--;

    if(type->cnt == 0)
    {
        type->cnt = 1;
        type->regions[0].base = 0;
        type->regions[0].size = 0;
    }
}

static void memblock_merge_regions(struct memblock_type *type)
{
    int i = 0;

    while (i < type->cnt - 1)
    {
        struct memblock_region *this = &type->regions[i];
        struct memblock_region *next = &type->regions[i + 1];

        if(this->base + this->size != next->base)
        {
            i++;
            continue;
        }

        this->size += next->size;

        memmove(next, next + 1, (type->cnt - (i + 2)) * sizeof(*next));
        type->cnt--;
    }
}

/**
 * @brief add new memblock region
 *
 * @param type
 * @param base
 * @param size
 * @return int
 */
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

repeat:
    base = obase;
    nr_new = 0;

    for_each_memblock_type(idx, type, rgn)
    {
        phys_addr_t rbase = rgn->base;
        phys_addr_t rend = rgn->base + rgn->size;

        // 新的region插入到该region前面
        if(rbase >= end)
        {
            break;
        }

        // 无法确定插入位置，继续向后查找 如果是最后一个region还是这样 那就插入到最后位置
        if(rend <= base)
        {
            continue;
        }

        /**
         *
         *
         *  new_region:
         *              base                        end
         *              |===========================|
         *                              rbase                                 rend
         *                              |=====================================|
         *
         * merge:       base  rbase - base rbase                               rend
         *              |===============| |=====================================|
         *
         */

        if(rbase > base)
        {
            nr_new++;

            if(insert)
            {
                memblock_insert_region(type, idx, base, rbase - base);
            }
        }

        base = min(rend, end);
    }

    if(base < end)
    {
        nr_new++;

        if(insert)
        {
            memblock_insert_region(type, idx, base, end-base);
        }
    }

    if(!nr_new)
        return 0;

    if(!insert)
    {
        if(type->cnt + nr_new > type->max)
        {
            return -ENOMEM;
        }

        insert = true;
        goto repeat;
    }
    else
    {
        memblock_merge_regions(type);
        return 0;
    }
}

int memblock_add(phys_addr_t base, phys_addr_t size)
{
    phys_addr_t end = base + size - 1;
    memblock_dbg("%s: [%pa-%pa]\n", __func__, &base, &end);

    return memblock_add_range(&memblock.memory, base, size);
}

static int memblock_isolate_range(struct memblock_type *type,
                                  phys_addr_t base, phys_addr_t size,
                                  int *start_rgn_idx, int *end_rgn_idx)
{
    phys_addr_t end = base + size;
    int idx;
    struct memblock_region *rgn;

    *start_rgn_idx = *end_rgn_idx = 0;

    if(!size)
    {
        return 0;
    }

    if(type->cnt + 2 > type->max)
    {
        return -ENOMEM;
    }

    for_each_memblock_type(idx, type, rgn)
    {
        phys_addr_t rbase = rgn->base;
        phys_addr_t rend = rgn->base + rgn->size;

        if(rbase >= end)
            break;

        if(rend <= base)
            continue;

        if(rbase < base)
        {
            rgn->base = base;
            rgn->size -= base - rbase;
            type->total_size -= base - rbase;

            memblock_insert_region(type, idx, rbase, base - rbase);
        }
        else if(rend > end)
        {
            rgn->base = end;
            rgn->size -= end - rbase;
            type->total_size -= end - rbase;

            memblock_insert_region(type, idx--, rbase, end - rbase);
        }
        else
        {
            if(!*end_rgn_idx)
            {
                *start_rgn_idx = idx;
            }

            *end_rgn_idx = idx + 1;
        }
    }

    return 0;
}

static int memblock_remove_range(struct memblock_type *type, phys_addr_t base, phys_addr_t size)
{
    int start_rgn_idx, end_rgn_idx;
    int i, ret;

    ret = memblock_isolate_range(type, base, size, &start_rgn_idx, &end_rgn_idx);
    if(ret)
        return ret;

    for(i = end_rgn_idx -1; i > start_rgn_idx; i--)
    {
        memblock_remove_region(type, i);
    }

    return 0;
}

int memblock_remove(phys_addr_t base, phys_addr_t size)
{
    phys_addr_t end = base + size - 1;
    memblock_dbg("%s: [%pa-%pa]\n", __func__, &base, &end);

    return memblock_remove_range(&memblock.memory, base, size);
}

int memblock_reserve(phys_addr_t base, phys_addr_t size)
{
    phys_addr_t end = base + size - 1;
    memblock_dbg("%s: [%pa-%pa]\n", __func__, &base, &end);
    return memblock_add_range(&memblock.reserved, base, size);
}

void __next_mem_range(u64 *idx,
                          struct memblock_type *type_a,
                          struct memblock_type *type_b,
                          phys_addr_t *out_start,
                          phys_addr_t *out_end)
{
    // 使用一个64位变量 低32位表示可用mem序号 高32位表示保留mem序号
    int idx_a = *idx & 0xffffffff;
    int idx_b = *idx >> 32;
    struct memblock_region *m;
    struct memblock_region *r;
    phys_addr_t m_start;
    phys_addr_t m_end;
    phys_addr_t r_start;
    phys_addr_t r_end;

    //  首先遍历可用mem空间 找到第一块可用空间
    for(; idx_a < type_a->cnt; idx_a++)
    {
        m = &type_a->regions[idx_a];

        m_start = m->base;
        m_end = m->base + m->size;

        if(!type_b)
        {
            if(out_start)
                *out_start = m_start;
            if(out_end)
                *out_end = m_end;

            idx_a++;
            *idx = (u32)idx_a | (u64)idx_b << 32;
            return;
        }


        for(; idx_b < type_b->cnt + 1; idx_b++)
        {
            r = &type_b->regions[idx_b];

            r_start = idx_b ? (r[-1].base + r[-1].size) : 0;
            r_end = (idx_b < type_b->cnt) ? r->base : PHYS_ADDR_MAX;

            if(r_start >= m_end)
            {
                break;
            }

            if(m_start < r_end)
            {
                if(out_start)
                    *out_start = max(m_start, r_start);
                if(out_end)
                    *out_end = min(m_end, r_end);

                if(m_end <= r_end)
                {
                    idx_a++;
                }
                else
                {
                    idx_b++;
                }

                *idx = (u32)idx_a | (u64)idx_b << 32;
                return;
            }
        }
    }


    *idx = ULLONG_MAX;
}

void __next_mem_range_rev(u64 *idx,
                          struct memblock_type *type_a,
                          struct memblock_type *type_b,
                          phys_addr_t *out_start,
                          phys_addr_t *out_end)
{
    int idx_a = *idx & 0xffffffff;
    int idx_b = *idx >> 32;

    if(*idx == (u64)ULLONG_MAX)
    {
        idx_a = type_a->cnt - 1;

        if(type_b != NULL)
            idx_b = type_b->cnt;
        else
            idx_b = 0;
    }

    for(; idx_a >= 0; idx_a--)
    {
        struct memblock_region *m = &type_a->regions[idx_a];

        phys_addr_t m_start = m->base;
        phys_addr_t m_end = m->base + m->size;

        if(!type_b)
        {
            if(out_start)
                *out_start = m_start;
            if(out_end)
                *out_end = m_end;

            idx_a--;
            *idx = (u32)idx_a | (u64)idx_b << 32;
            return;
        }

        for(; idx_b >= 0; idx_b--)
        {
            struct memblock_region *r;
            phys_addr_t r_start;
            phys_addr_t r_end;

            r = &type_b->regions[idx_b];
            r_start = idx_b ? r[-1].base + r[-1].size : 0;
            r_end = idx_b < type_b->cnt ? r->base : PHYS_ADDR_MAX;

            if(r_end <= m_start)
                break;

            if(m_end > r_start)
            {
                if(out_start)
                    *out_start = max(m_start, r_start);
                if(out_end)
                    *out_end = min(m_end, r_end);
                if(m_start >= r_start)
                    idx_a--;
                else
                    idx_b--;
                *idx = (u32)idx_a | (u64)idx_b << 32;
                return;
            }
        }
    }

    /* signal end of iteration */
    *idx = ULLONG_MAX;
}

static phys_addr_t memblock_find(phys_addr_t size, phys_addr_t align)
{
    phys_addr_t this_start, this_end, cand;
    u64 i;

    for_each_free_mem_range_reverse(i, &this_start, &this_end)
    {
        if(this_end < size)
            continue;

        cand = round_down(this_end - size, align);
        if(cand >= this_start)
            return cand;
    }

    return 0;
}

phys_addr_t memblock_phys_alloc_align(phys_addr_t size, phys_addr_t align)
{
    phys_addr_t found;

    found = memblock_find(size, align);

    if(found && !memblock_reserve(found, size))
        return found;

    return 0;
}

int memblock_phys_free(phys_addr_t base, phys_addr_t size)
{
    phys_addr_t end = base + size - 1;
    memblock_dbg("%s: [%pa-%pa]\n", __func__, &base, &end);

    return memblock_remove_range(&memblock.reserved, base, size);
}

void *memblock_alloc(phys_addr_t size, phys_addr_t align)
{

    memblock_dbg("%s: %llu bytes align=0x%llx\n",
             __func__, (u64)size, (u64)align);

    phys_addr_t alloc;

    alloc = memblock_phys_alloc_align(size, align);

    if(!alloc)
        return NULL;

    return phys_to_virt(alloc);
}

void memblock_free(void *ptr, size_t size)
{
    if(ptr)
        memblock_phys_free(__pa(ptr), size);
}

void __free_mem_core(phys_addr_t start, phys_addr_t end)
{
	int order;
	unsigned long start_pfn = PFN_UP(start);
	unsigned long end_pfn = PFN_DOWN(end);

	if(start_pfn >= end_pfn)
		return;

	while(start_pfn < end_pfn)
	{
		/* 内核使用 MAX_ORDER - 1. 没有想清楚是因为什么 */
		/* 难道是因为这个是指 阶 的个数 (0 - 10) 一共有11个阶 */
		order = min(MAX_ORDER - 1, __ffs(start));

		while((start_pfn + (1 << order)) > end_pfn)
		{
			order--;
		}

		memblock_free_pages(start_pfn, order);
		start_pfn += (1 << order);
	}
}

/*
 * free memblock free memory to buddy page
 */
void free_memory_core(void)
{
	u64 i;
	phys_addr_t start, end;

	for_each_free_mem_range(i, &start, &end)
	{
		__free_mem_core(start, end);
	}
}

static void memblock_dump(struct memblock_type *type)
{
    struct memblock_region *rgn;
    int idx;
    phys_addr_t base, end, size;

    printk("%s.cnt = 0x%lx\n", type->name, type->cnt);

    for_each_memblock_type(idx, type, rgn)
    {
        rgn = &type->regions[idx];

        base = rgn->base;
        size = rgn->size;
        end = base + size;

        printk(" %s[%#x]\t[0x%016x-%016x], 0x%016x bytes\n",
            type->name, idx, base, end, size);
    }

}

void memblock_dump_all(void)
{
    printk("MEMBLOCK configuration:\n");
    printk(" memory size = 0x%016x reserved size = 0x%016x\n",
        memblock.memory.total_size,
        memblock.reserved.total_size);

    memblock_dump(&memblock.memory);
    memblock_dump(&memblock.reserved);
}
