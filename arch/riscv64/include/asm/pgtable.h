#ifndef _ASM_RISCV_PGTABLE_H
#define _ASM_RISCV_PGTABLE_H

#include <const.h>
#include <asm/pgtable-bits.h>

#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE - 1))

#define PGDIR_SHIFT     30
/* Size of region mapped by a page global directory */
//1G SIZE
#define PGDIR_SIZE      (_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK      (~(PGDIR_SIZE - 1))

#define PMD_SHIFT       21
/* Size of region mapped by a page middle directory */
//2M SIZE
#define PMD_SIZE        (_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SIZE - 1))

/* Number of entries in the page global directory */
#define PTRS_PER_PGD    (PAGE_SIZE / sizeof(pgd_t))
/* Number of entries in the page middle directory */
#define PTRS_PER_PMD    (PAGE_SIZE / sizeof(pmd_t))
/* Number of entries in the page table */
#define PTRS_PER_PTE    (PAGE_SIZE / sizeof(pte_t))

#ifndef __ASSEMBLY__

/* Page Global Directory entry */
typedef struct {
	unsigned long pgd;
} pgd_t;

typedef struct {
	unsigned long pmd;
} pmd_t;

/* Page Table entry */
typedef struct {
	unsigned long pte;
} pte_t;

typedef struct {
	unsigned long pgprot;
} pgprot_t;

#define pte_val(x)	((x).pte)
#define pmd_val(x)	((x).pmd)
#define pgd_val(x)	((x).pgd)
#define pgprot_val(x)	((x).pgprot)

#define __pte(x)	((pte_t) { (x) })
#define __pmd(x)	((pmd_t) { (x) })
#define __pgd(x)	((pgd_t) { (x) })
#define __pgprot(x)	((pgprot_t) { (x) })

#include <asm-generic/pgtable-nopud.h>

static inline pgd_t pfn_pgd(unsigned long pfn, pgprot_t prot)
{
	return __pgd((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot));
}

static inline unsigned long _pgd_pfn(pgd_t pgd)
{
	return pgd_val(pgd) >> _PAGE_PFN_SHIFT;
}

static inline pmd_t pfn_pmd(unsigned long pfn, pgprot_t prot)
{
	return __pmd((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot));
}

static inline unsigned long _pmd_pfn(pmd_t pmd)
{
	return pmd_val(pmd) >> _PAGE_PFN_SHIFT;
}

/* Yields the page frame number (PFN) of a page table entry */
static inline unsigned long pte_pfn(pte_t pte)
{
	return (pte_val(pte) >> _PAGE_PFN_SHIFT);
}

#define pte_page(x)     pfn_to_page(pte_pfn(x))

/* Constructs a page table entry */
static inline pte_t pfn_pte(unsigned long pfn, pgprot_t prot)
{
	return __pte((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot));
}

/* Page protection bits */
#define _PAGE_BASE	(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_USER)

#define PAGE_NONE		__pgprot(_PAGE_PROT_NONE)
#define PAGE_READ		__pgprot(_PAGE_BASE | _PAGE_READ)
#define PAGE_WRITE		__pgprot(_PAGE_BASE | _PAGE_READ | _PAGE_WRITE)
#define PAGE_EXEC		__pgprot(_PAGE_BASE | _PAGE_EXEC)
#define PAGE_READ_EXEC		__pgprot(_PAGE_BASE | _PAGE_READ | _PAGE_EXEC)
#define PAGE_WRITE_EXEC		__pgprot(_PAGE_BASE | _PAGE_READ |	\
					 _PAGE_EXEC | _PAGE_WRITE)

#define _PAGE_KERNEL		(_PAGE_READ \
				| _PAGE_WRITE \
				| _PAGE_PRESENT \
				| _PAGE_ACCESSED \
				| _PAGE_DIRTY \
				| _PAGE_GLOBAL)

#define PAGE_KERNEL		__pgprot(_PAGE_KERNEL)
#define PAGE_KERNEL_READ	__pgprot(_PAGE_KERNEL & ~_PAGE_WRITE)
#define PAGE_KERNEL_EXEC	__pgprot(_PAGE_KERNEL | _PAGE_EXEC)
#define PAGE_KERNEL_READ_EXEC	__pgprot((_PAGE_KERNEL & ~_PAGE_WRITE) \
					 | _PAGE_EXEC)
#define PAGE_TABLE		__pgprot(_PAGE_TABLE)

static inline int pmd_present(pmd_t pmd)
{
	return (pmd_val(pmd) & (_PAGE_PRESENT | _PAGE_PROT_NONE));
}

static inline int pmd_none(pmd_t pmd)
{
	return (pmd_val(pmd) == 0);
}

static inline int pmd_bad(pmd_t pmd)
{
	return !pmd_present(pmd) || (pmd_val(pmd) & _PAGE_LEAF);
}

static inline int pte_present(pte_t pte)
{
	return (pte_val(pte) & (_PAGE_PRESENT | _PAGE_PROT_NONE));
}

static inline int pte_none(pte_t pte)
{
	return (pte_val(pte) == 0);
}

static inline int pte_write(pte_t pte)
{
	return pte_val(pte) & _PAGE_WRITE;
}

static inline int pte_exec(pte_t pte)
{
	return pte_val(pte) & _PAGE_EXEC;
}

/*
 * Certain architectures need to do special things when PTEs within
 * a page table are directly modified.  Thus, the following hook is
 * made available.
 */
static inline void set_pte(pte_t *ptep, pte_t pteval)
{
	*ptep = pteval;
}

#endif
#endif

