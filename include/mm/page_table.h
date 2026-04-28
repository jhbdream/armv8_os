#ifndef _MM_PAGE_TABLE_H
#define _MM_PAGE_TABLE_H

#include <stdint.h>

typedef uint64_t pgd_t;
typedef uint64_t pud_t;
typedef uint64_t pmd_t;
typedef uint64_t pte_t;

/* 页面保护属性 */
#define PROT_NONE  0
#define PROT_READ  (1UL << 0)
#define PROT_WRITE (1UL << 1)
#define PROT_EXEC  (1UL << 2)
#define PROT_USER  (1UL << 3)
#define PROT_DEV   (1UL << 4)

/* 组合属性 */
#define PAGE_KERNEL    (PROT_READ | PROT_WRITE | PROT_EXEC)
#define PAGE_KERNEL_RO (PROT_READ | PROT_EXEC)
#define PAGE_KERNEL_NC (PROT_READ | PROT_WRITE | PROT_EXEC)
#define PAGE_DEVICE    (PROT_READ | PROT_WRITE)

int map_page(pgd_t *pgd, unsigned long va, unsigned long pa, unsigned long prot);
void unmap_page(pgd_t *pgd, unsigned long va);
int map_region(pgd_t *pgd, unsigned long va, unsigned long pa, unsigned long size, unsigned long prot);
void unmap_region(pgd_t *pgd, unsigned long va, unsigned long size);

#endif
