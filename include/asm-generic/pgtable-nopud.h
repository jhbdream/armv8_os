#ifndef _PGTABLE_NOPUD_H
#define _PGTABLE_NOPUD_H

#ifndef __ASSEMBLY__

typedef struct { pgd_t pgd; } pud_t;

#define PUD_SHIFT	PGDIR_SHIFT
#define PTRS_PER_PUD	1
#define PUD_SIZE  	(1UL << PUD_SHIFT)
#define PUD_MASK  	(~(PUD_SIZE-1))

static inline pud_t *pud_offset(pgd_t *pgd, unsigned long address)
{
	return (pud_t *)pgd;
}
#define pud_offset pud_offset

#define pud_val(x)				(pgd_val((x).pgd))
#define __pud(x)				((pud_t) { __pgd(x) })

#endif /* __ASSEMBLY */

#endif /* _PGTABLE_NOPUD_H */
