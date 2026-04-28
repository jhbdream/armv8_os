#include <stddef.h>
#include <stdint.h>

#include <io.h>
#include <asm/memory.h>
#include <mm/page.h>
#include <mm/page_alloc.h>
#include <mm/page_table.h>

#define __aligned(x) __attribute__((__aligned__(x)))

/*
 * Hardware page table definitions.
 *
 * Level -1 descriptor (PGD).
 */
#define PGD_TYPE_TABLE (3UL << 0)
#define PGD_TABLE_BIT  (1UL << 1)
#define PGD_TYPE_MASK  (3UL << 0)
#define PGD_TABLE_AF   (1UL << 10)
#define PGD_TABLE_PXN  (1UL << 59)
#define PGD_TABLE_UXN  (1UL << 60)

/*
 * Level 1 descriptor (PUD).
 */
#define PUD_TYPE_TABLE  (3UL << 0)
#define PUD_TABLE_BIT   (1UL << 1)
#define PUD_TYPE_MASK   (3UL << 0)
#define PUD_TYPE_SECT   (1UL << 0)
#define PUD_SECT_RDONLY (1UL << 7)
#define PUD_TABLE_AF    (1UL << 10)
#define PUD_TABLE_PXN   (1UL << 59)
#define PUD_TABLE_UXN   (1UL << 60)

/*
 * Level 2 descriptor (PMD).
 */
#define PMD_TYPE_MASK  (3UL << 0)
#define PMD_TYPE_TABLE (3UL << 0)
#define PMD_TYPE_SECT  (1UL << 0)
#define PMD_TABLE_BIT  (1UL << 1)
#define PMD_TABLE_AF   (1UL << 10)

/*
 * Section
 */
#define PMD_SECT_USER   (1UL << 6)
#define PMD_SECT_RDONLY (1UL << 7)
#define PMD_SECT_S      (3UL << 8)
#define PMD_SECT_AF     (1UL << 10)
#define PMD_SECT_NG     (1UL << 11)
#define PMD_SECT_CONT   (1UL << 52)
#define PMD_SECT_PXN    (1UL << 53)
#define PMD_SECT_UXN    (1UL << 54)
#define PMD_TABLE_PXN   (1UL << 59)
#define PMD_TABLE_UXN   (1UL << 60)

#define PMD_ATTRINDX(t) ((t) << 2)

/*
 * Level 3 descriptor (PTE).
 */
#define PTE_VALID     (1UL << 0)
#define PTE_TYPE_MASK (3UL << 0)
#define PTE_TYPE_PAGE (3UL << 0)
#define PTE_TABLE_BIT (1UL << 1)
#define PTE_USER      (1UL << 6)
#define PTE_RDONLY    (1UL << 7)
#define PTE_SHARED    (3UL << 8)
#define PTE_AF        (1UL << 10)
#define PTE_NG        (1UL << 11)
#define PTE_GP        (1UL << 50)
#define PTE_DBM       (1UL << 51)
#define PTE_CONT      (1UL << 52)
#define PTE_PXN       (1UL << 53)
#define PTE_UXN       (1UL << 54)

#define PMD_SHIFT     (21)
#define PMD_SIZE      (1UL << PMD_SHIFT)
#define PMD_MASK      (~(PMD_SIZE - 1))
#define PTRS_PER_PMD  (512)

#define PUD_SHIFT     (30)
#define PUD_SIZE      (1UL << PUD_SHIFT)
#define PUD_MASK      (~(PUD_SIZE - 1))
#define PTRS_PER_PUD  (512)

#define PGDIR_SHIFT   (39)
#define PGDIR_SIZE    (1UL << PGDIR_SHIFT)
#define PGDIR_MASK    (~(PGDIR_SIZE - 1))
#define PTRS_PER_PGD  (512)

#define PTRS_PER_PTE  (512)
#define PTE_ADDR_MASK PAGE_MASK

#define PGD_INDEX(va) (((va) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define PUD_INDEX(va) (((va) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define PMD_INDEX(va) (((va) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define PTE_INDEX(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

uint64_t kimage_voffset = 0;

uint64_t pgd_table[PTRS_PER_PGD] __aligned(4096);
static uint64_t pud_table[PTRS_PER_PUD] __aligned(4096);
static uint64_t pmd_table[PTRS_PER_PMD * 16] __aligned(4096);

static void init_pagetables(void)
{
    static int init_pagetables_done = 0;

    int i;
    uint64_t va;
    uint64_t pgd_phys;
    uint64_t pud_phys;
    uint64_t pmd_phys;
    uint64_t *pud_entry;
    uint64_t *pgd_entry;

    if (init_pagetables_done != 0)
        return;

    va = VIRT_OFFSET;

    pgd_phys = virt_to_phys(pgd_table);
    pud_phys = virt_to_phys(pud_table);
    pmd_phys = virt_to_phys(pmd_table);

    pgd_entry = &pgd_table[PGD_INDEX(va)];
    writeq(pud_phys | PGD_TYPE_TABLE, pgd_entry);

    for (i = 0; i < 16; i++) {
        pud_entry = &pud_table[PUD_INDEX(va)];
        writeq(pmd_phys | PUD_TYPE_TABLE, pud_entry);
        va += PUD_SIZE;
        pmd_phys += 4096;
    }

    init_pagetables_done = 1;
}

static void create_simple_map(uint64_t va, uint64_t pa, uint64_t size, uint64_t type)
{
    uint64_t va_end;
    uint64_t pmd;
    uint64_t *pud_entry;
    uint64_t *pmd_entry;
    uint64_t *pmd_page_table;

    va_end = va + size;

    while (va < va_end) {
        pud_entry = &pud_table[PUD_INDEX(va)];
        pmd_page_table = (uint64_t *)__phys_to_kimg(*pud_entry & 0xFFFFFFFFFFFFF000);
        pmd_entry = &pmd_page_table[PMD_INDEX(va)];

        pmd = pa
              | PMD_SECT_AF
              | PMD_ATTRINDX(type)
              | PMD_SECT_S | PMD_TYPE_SECT;

        writeq(pmd, pmd_entry);

        va += PMD_SIZE;
        pa += PMD_SIZE;
    }
}

static void switch_mm(uint64_t *pgd)
{
    phys_addr_t pgd_phys;

    pgd_phys = virt_to_phys(pgd);

    asm volatile("tlbi vmalle1is");
    asm volatile("dsb sy");
    asm volatile("isb");
    asm volatile("msr ttbr1_el1, %0" ::"r"(pgd_phys));
    asm volatile("dsb sy");
    asm volatile("isb");
}

extern unsigned long __kimage_start[], __kimage_end[];

void create_kernel_map(void)
{
    uint64_t va, pa, size;

    init_pagetables();

    va   = (uint64_t)__kimage_start;
    pa   = (uint64_t)__kimage_start - kimage_voffset;
    size = (uint64_t)__kimage_end - (uint64_t)__kimage_start;

    create_simple_map(va, pa, size, MT_NORMAL);

    va   = PAGE_OFFSET;
    pa   = PHYS_OFFSET;
    size = PHYS_SIZE;

    create_simple_map(va, pa, size, MT_NORMAL);

    va   = DEVICE_START;
    pa   = DEVICE_PHYS_OFFSET;
    size = DEVICE_PHYS_SIZE;
    create_simple_map(va, pa, size, MT_DEVICE_nGnRnE);

    switch_mm(pgd_table);
}

/* ---- Page table management API ---- */

static unsigned long prot_to_pte_flags(unsigned long prot)
{
    unsigned long flags = PTE_VALID | PTE_AF | PTE_SHARED;

    if (prot & PROT_USER)
        flags |= PTE_USER;
    if (!(prot & PROT_WRITE))
        flags |= PTE_RDONLY;
    if (!(prot & PROT_EXEC))
        flags |= PTE_PXN | PTE_UXN;

    return flags;
}

static pte_t *alloc_pte_table(void)
{
    struct page *page = alloc_pages(0);
    if (!page)
        return NULL;

    pte_t *table = (pte_t *)page_to_virt(page);
    for (int i = 0; i < PTRS_PER_PTE; i++)
        table[i] = 0;
    return table;
}

static pmd_t *alloc_pmd_table(void)
{
    struct page *page = alloc_pages(0);
    if (!page)
        return NULL;

    pmd_t *table = (pmd_t *)page_to_virt(page);
    for (int i = 0; i < PTRS_PER_PMD; i++)
        table[i] = 0;
    return table;
}

static pte_t *walk_pte(pgd_t *pgd, unsigned long va, int alloc)
{
    unsigned int pgd_idx = PGD_INDEX(va);
    unsigned int pud_idx = PUD_INDEX(va);
    unsigned int pmd_idx = PMD_INDEX(va);
    pmd_t pmd_val;
    pud_t *pud_tbl;
    pmd_t *pmd_tbl;
    pte_t *pte_tbl;

    if (!(pgd[pgd_idx] & PGD_TYPE_TABLE))
        return NULL;
    pud_tbl = (pud_t *)phys_to_virt(pgd[pgd_idx] & PTE_ADDR_MASK);

    if (!(pud_tbl[pud_idx] & PUD_TYPE_TABLE))
        return NULL;
    pmd_tbl = (pmd_t *)phys_to_virt(pud_tbl[pud_idx] & PTE_ADDR_MASK);

    pmd_val = pmd_tbl[pmd_idx];
    if (!(pmd_val & PMD_TYPE_TABLE)) {
        if (!alloc)
            return NULL;

        pte_t *new_pte = alloc_pte_table();
        if (!new_pte)
            return NULL;

        writeq(virt_to_phys(new_pte) | PMD_TYPE_TABLE, &pmd_tbl[pmd_idx]);
        return &new_pte[PTE_INDEX(va)];
    }

    pte_tbl = (pte_t *)phys_to_virt(pmd_val & PTE_ADDR_MASK);
    return &pte_tbl[PTE_INDEX(va)];
}

int map_page(pgd_t *pgd, unsigned long va, unsigned long pa, unsigned long prot)
{
    pte_t *pte = walk_pte(pgd, va, 1);
    if (!pte)
        return -1;

    writeq(pa | prot_to_pte_flags(prot) | PTE_TYPE_PAGE, pte);
    return 0;
}

void unmap_page(pgd_t *pgd, unsigned long va)
{
    pte_t *pte = walk_pte(pgd, va, 0);
    if (pte)
        writeq(0, pte);
}

int map_region(pgd_t *pgd, unsigned long va, unsigned long pa, unsigned long size, unsigned long prot)
{
    unsigned long end = va + size;

    while (va < end) {
        if (map_page(pgd, va, pa, prot) < 0)
            return -1;
        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }
    return 0;
}

void unmap_region(pgd_t *pgd, unsigned long va, unsigned long size)
{
    unsigned long end = va + size;

    while (va < end) {
        unmap_page(pgd, va);
        va += PAGE_SIZE;
    }

    asm volatile("dsb sy");
    asm volatile("tlbi vmalle1is");
    asm volatile("dsb sy");
    asm volatile("isb");
}
