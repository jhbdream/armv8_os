#ifndef _MMU_H
#define _MMU_H

#include <asm/pgtable_type.h>

void early_fixmap_init(pgd_t *pg_dir);
phys_addr_t early_pgtable_alloc(int shift);
void create_pgd_mapping(pgd_t *pgdir, phys_addr_t phys, unsigned long virt,
			phys_addr_t size, pgprot_t prot,
			phys_addr_t (*pgtable_alloc)(int), int flags);
void paging_init(void);

#endif