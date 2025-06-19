#include <stdint.h>

#include <io.h>
#include <asm/memory.h>

#include <eeos/types.h>
#include <eeos/const.h>

#define __aligned(x) __attribute__((__aligned__(x)))

/*
 * Hardware page table definitions.
 *
 * Level -1 descriptor (PGD).
 */
#define PGD_TYPE_TABLE (3 << 0)
#define PGD_TABLE_BIT  (1 << 1)
#define PGD_TYPE_MASK  (3 << 0)
#define PGD_TABLE_AF   (1 << 10) /* Ignored if no FEAT_HAFT */
#define PGD_TABLE_PXN  (1 << 59)
#define PGD_TABLE_UXN  (1 << 60)

/*
 * Level 1 descriptor (PUD).
 */
#define PUD_TYPE_TABLE  (3 << 0)
#define PUD_TABLE_BIT   (1 << 1)
#define PUD_TYPE_MASK   (3 << 0)
#define PUD_TYPE_SECT   (1 << 0)
#define PUD_SECT_RDONLY (1 << 7)  /* AP[2] */
#define PUD_TABLE_AF    (1 << 10) /* Ignored if no FEAT_HAFT */
#define PUD_TABLE_PXN   (1 << 59)
#define PUD_TABLE_UXN   (1 << 60)

/*
 * Level 2 descriptor (PMD).
 */
#define PMD_TYPE_MASK  (3 << 0)
#define PMD_TYPE_TABLE (3 << 0)
#define PMD_TYPE_SECT  (1 << 0)
#define PMD_TABLE_BIT  (1 << 1)
#define PMD_TABLE_AF   (1 << 10) /* Ignored if no FEAT_HAFT */

/*
 * Section
 */
#define PMD_SECT_USER   (1 << 6) /* AP[1] */
#define PMD_SECT_RDONLY (1 << 7) /* AP[2] */
#define PMD_SECT_S      (3 << 8)
#define PMD_SECT_AF     (1 << 10)
#define PMD_SECT_NG     (1 << 11)
#define PMD_SECT_CONT   (1 << 52)
#define PMD_SECT_PXN    (1 << 53)
#define PMD_SECT_UXN    (1 << 54)
#define PMD_TABLE_PXN   (1 << 59)
#define PMD_TABLE_UXN   (1 << 60)

#define PMD_ATTRINDX(t) ((t) << 2)

/*
 * Level 3 descriptor (PTE).
 */
#define PTE_VALID     (1 << 0)
#define PTE_TYPE_MASK (3 << 0)
#define PTE_TYPE_PAGE (3 << 0)
#define PTE_TABLE_BIT (1 << 1)
#define PTE_USER      (1 << 6)  /* AP[1] */
#define PTE_RDONLY    (1 << 7)  /* AP[2] */
#define PTE_SHARED    (3 << 8)  /* SH[1:0], inner shareable */
#define PTE_AF        (1 << 10) /* Access Flag */
#define PTE_NG        (1 << 11) /* nG */
#define PTE_GP        (1 << 50) /* BTI guarded */
#define PTE_DBM       (1 << 51) /* Dirty Bit Management */
#define PTE_CONT      (1 << 52) /* Contiguous range */
#define PTE_PXN       (1 << 53) /* Privileged XN */
#define PTE_UXN       (1 << 54) /* User XN */

#define PAGE_SHIFT    (12)
#define PAGE_SIZE     (_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK     (~(PAGE_SIZE - 1))
#define PTE_ADDR_MASK (PAGE_MASK)
#define PTRS_PER_PTE  (512)

#define PMD_SHIFT     (21)
#define PMD_SIZE      (_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK      (~(PMD_SIZE - 1))
#define PTRS_PER_PMD  (512)

#define PUD_SHIFT     (30)
#define PUD_SIZE      (_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK      (~(PUD_SIZE - 1))
#define PTRS_PER_PUD  (512)

#define PGDIR_SHIFT   (39)
#define PGDIR_SIZE    (_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK    (~(PGDIR_SIZE - 1))
#define PTRS_PER_PGD  (512)

#define PGD_INDEX(va) (((va) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define PUD_INDEX(va) (((va) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define PMD_INDEX(va) (((va) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define PTE_INDEX(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

uint64_t kimage_voffset = 0;

static uint64_t pgd_table[PTRS_PER_PGD] __aligned(4096);
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

    // 1. 获取页表物理地址

    pgd_phys = virt_to_phys(pgd_table);
    pud_phys = virt_to_phys(pud_table);
    pmd_phys = virt_to_phys(pmd_table);

    // 2. 配置PGD表项 -> PUD表
    pgd_entry = &pgd_table[PGD_INDEX(va)];
    writeq(pud_phys | PGD_TYPE_TABLE, pgd_entry);

    // 3. 配置PUD表项 -> PMD表

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

        pmd = pa                            // 块描述符
              | PMD_SECT_AF                 // Access Flag
              | PMD_ATTRINDX(type)          // 内存类型
              | PMD_SECT_S | PMD_TYPE_SECT; // 可共享

        writeq(pmd, pmd_entry);

        va += PMD_SIZE; // 增加2MB虚拟地址
        pa += PMD_SIZE; // 增加2MB物理地址
    }
}

static void switch_mm(uint64_t *pgd)
{
    phys_addr_t pgd_phys;

    pgd_phys = virt_to_phys(pgd);

    // 5. 刷新TLB并设置TTBR1_EL1
    asm volatile("tlbi vmalle1is"); // 无效化所有TLB条目
    asm volatile("dsb sy");
    asm volatile("isb");
    asm volatile("msr ttbr1_el1, %0" ::"r"(pgd_phys)); // 设置页表基址
    asm volatile("dsb sy");
    asm volatile("isb");
}

extern unsigned long __kimage_start[], __kimage_end[];

/**
 * @brief 创建3级页表，内核映射线性 + 镜像映射
 */
void create_kernel_map(void)
{
    uint64_t va, pa, size;

    init_pagetables();

    // 创建内核镜像映射
    va   = (uint64_t)__kimage_start;
    pa   = (uint64_t)__kimage_start - kimage_voffset;
    size = (uint64_t)__kimage_end - (uint64_t)__kimage_start;

    create_simple_map(va, pa, size, MT_NORMAL);

    // 创建1G线性映射
    va   = PAGE_OFFSET;
    pa   = PHYS_OFFSET;
    size = PHYS_SIZE;

    create_simple_map(va, pa, size, MT_NORMAL);

    // 创建DEVICE映射
    va   = DEVICE_START;
    pa   = DEVICE_PHYS_OFFSET;
    size = DEVICE_PHYS_SIZE;
    create_simple_map(va, pa, size, MT_DEVICE_nGnRnE);

    switch_mm(pgd_table);
}
