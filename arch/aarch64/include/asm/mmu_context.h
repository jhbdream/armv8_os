#ifndef _MMU_CONTEXT_H
#define _MMU_CONTEXT_H

#include "pgtable_type.h"
#include "memory.h"

static inline void cpu_replace_ttbr1(pgd_t *pgdp)
{
    typedef void (ttbr_replace_func)(phys_addr_t);
    extern ttbr_replace_func idmap_cpu_replace_ttbr1;
    ttbr_replace_func *replace_phys;

    phys_addr_t ttbr1 = virt_to_phys(pgdp);
    replace_phys = (ttbr_replace_func *)__pa_symbol(idmap_cpu_replace_ttbr1);

    replace_phys(ttbr1);
}

#endif