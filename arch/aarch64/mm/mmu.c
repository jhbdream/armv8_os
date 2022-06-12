#include "pgtable_hwdef.h"
#include <memory.h>
#include <printk.h>
#include <type.h>
#include <pgtable.h>
#include <asm/pgtable_type.h>
#include <asm/fixmap.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <compiler_attribute.h>
#include <ee/pgtable.h>
#include <mm/memblock.h>
#include <asm/mmu_context.h>

uint64_t kimage_voffset;

static pte_t bm_pte[PTRS_PER_PTE] __aligned(PAGE_SIZE);
static pmd_t bm_pmd[PTRS_PER_PMD] __aligned(PAGE_SIZE);
static pud_t bm_pud[PTRS_PER_PUD] __aligned(PAGE_SIZE);

extern pgd_t swapper_pg_dir[PTRS_PER_PGD];

static inline pte_t *fixmap_pte(unsigned long addr)
{
     return &bm_pte[pte_index(addr)];
}

void __set_fixmap(enum fixed_addresses idx,
                  phys_addr_t phys, pgprot_t flags)
{
    unsigned long addr = fix_to_virt(idx);
    pte_t *ptep;

    ptep = fixmap_pte(addr);
    *ptep = pfn_pte(phys >> PAGE_SHIFT, flags);

    // 切换页表映射关系之后 需要进行刷新tlb操作 否则无法访问正确的物理地址
    asm volatile("tlbi vmalle1");
    asm volatile("isb");
}

phys_addr_t early_pgtable_alloc(int shift)
{
    phys_addr_t phys;

    phys = memblock_phys_alloc_align(PAGE_SIZE, PAGE_SIZE);
    return phys;
}

static inline bool use_1G_block(unsigned long addr, unsigned long next, unsigned long phys)
{
  if (((addr | next | phys) & ~PUD_MASK) != 0)
    return false;

  return true;
}

void pud_set_huge(pud_t *pud, phys_addr_t phys, pgprot_t prot)
{
    pud_t new_pud = pfn_pud((phys >> PAGE_SHIFT), mk_pud_sect_prot(prot));
    *pud = new_pud;

    asm volatile("tlbi vmalle1");
    asm volatile("isb");
}

void pmd_set_huge(pmd_t *pmd, phys_addr_t phys, pgprot_t prot)
{
    pmd_t new_pmd = pfn_pmd((phys >> PAGE_SHIFT), mk_pmd_sect_prot(prot));
    *pmd = new_pmd;

    asm volatile("tlbi vmalle1");
    asm volatile("isb");
}

void early_fixmap_init(pgd_t *pg_dir)
{
    pgd_t *pgdp;
    pgd_t pgd;
    pud_t *pudp;
    pmd_t *pmdp;
    unsigned long addr = FIXADDR_START;

    pgdp = pgd_offset_pgd(pg_dir, addr);
    pgd = *pgdp;

    // pgd空时创建页表
    if (pgd_none(pgd))
    {
        __pgd_populate(pgdp, __pa_symbol(bm_pud), PGD_TYPE_TABLE);
        pudp = pud_offset_pud(bm_pud, addr);
    }
    else
    {
        // 如果已经创建了pud，获取pudp虚拟地址
        pudp = (pud_t *)(__phys_to_kimg(__pgd_to_phys(pgd)));
        pudp = pud_offset_pud(pudp, addr);
    }

    if (pud_none(*pudp))
    {
        __pud_populate(pudp, __pa_symbol(bm_pmd), PUD_TYPE_TABLE);
        pmdp = pmd_offset_pmd(bm_pmd, addr);
    }
    else
    {
        pmdp = (pmd_t *)(__phys_to_kimg(__pud_to_phys(*pudp)));
        pmdp = pmd_offset_pmd(pmdp, addr);
    }

    if(pmd_none(*pmdp))
    {
        __pmd_populate(pmdp, __pa_symbol(bm_pte), PMD_TYPE_TABLE);
    }
    else
    {
        // wrony
    }

    //bm_pte 4K * 512 map to fixaddr
}

static void alloc_init_pte(pmd_t *pmdp, unsigned long addr, unsigned long end,
                           phys_addr_t phys, pgprot_t prot,
                           phys_addr_t (*pgtable_alloc)(int),
                           int flags)
{
    unsigned long next;
    pte_t *ptep;
    pmd_t pmd = *pmdp;

    if(pmd_none(pmd))
    {
        pmdval_t pmdval = PMD_TYPE_TABLE;
        phys_addr_t pte_phys;

        pte_phys = pgtable_alloc(PAGE_SHIFT);
        __pmd_populate(pmdp, pte_phys, pmdval);
        pmd = *pmdp;
    }

    ptep = pte_set_fixmap(pmdp, addr);

    do
    {
        *ptep = pfn_pte((phys >> PAGE_SHIFT), prot);
        phys += PAGE_SIZE;
    } while (ptep++, addr += PAGE_SIZE, addr != end);

}

static void alloc_init_pmd(pud_t *pudp, unsigned long addr, unsigned long end,
                           phys_addr_t phys, pgprot_t prot,
                           phys_addr_t (*pgtable_alloc)(int),
                           int flags)
{
    unsigned long next;
    pmd_t *pmdp;
    pud_t pud = *pudp;

    if(pud_none(pud))
    {
        pudval_t pudval = PUD_TYPE_TABLE;
        phys_addr_t pmd_phys;

        pmd_phys = pgtable_alloc(PMD_SHIFT);
        __pud_populate(pudp, pmd_phys, pudval);
        pud = *pudp;
    }

    pmdp = pmd_set_fixmap(pudp, addr);
    do
    {
        next = pmd_addr_end(addr, end);

        if(((addr | phys | next) & ~PMD_MASK) == 0)
        {
            pmd_set_huge(pmdp, phys, prot);
        }
        else {
            alloc_init_pte(pmdp, addr, next, phys, prot, pgtable_alloc, flags);
        }

        phys += next - addr;
    } while (pmdp++, addr = next, addr != end);
}


static void alloc_init_pud(pgd_t *pgdp, unsigned long addr, unsigned long end,
                           phys_addr_t phys, pgprot_t prot,
                           phys_addr_t (*pgtable_alloc)(int),
                           int flags)
{
    unsigned long next;
    pud_t *pudp;
    pgd_t pgd = *pgdp;

    if (pgd_none(pgd))
    {
        pgdval_t pgdval = PGD_TYPE_TABLE;
        phys_addr_t pud_phys;
        pud_phys = pgtable_alloc(PUD_SHIFT);
        __pgd_populate(pgdp, pud_phys, pgdval);
        pgd = *pgdp;
    }

    // use fix map do temp map for pud page alloc from memblock
    pudp = pud_set_fixmap(pgdp, addr);

    do
    {
        next = pud_addr_end(addr, end);

        if(use_1G_block(addr, next, phys))
        {
            pud_set_huge(pudp, phys, prot);
        }
        else
        {
            alloc_init_pmd(pudp, addr, end, phys, prot, pgtable_alloc, flags);
        }

        phys += next - addr;
    } while (pudp++, addr = next, addr != end);
}

void create_pgd_mapping(pgd_t *pgdir, phys_addr_t phys,
                                 unsigned long virt, phys_addr_t size,
                                 pgprot_t prot,
                                 phys_addr_t (*pgtable_alloc)(int),
                                 int flags)
{
    unsigned long addr, end, next;
    (void)addr;
    (void)end;

    pgd_t *pgdp = pgd_offset_pgd(pgdir, virt);

    //TODO: check phys and virt has same offset in a page
    // make sure (phys ^ virt) & ~PAGE_MASK) == 0

    phys = phys & PAGE_MASK;
    addr = virt & PAGE_MASK;
    end = addr + size;

    do
    {
        next = pgd_addr_end(addr, end);
        alloc_init_pud(pgdp, addr, next, phys, prot, pgtable_alloc, flags);
        phys += next - addr;
    } while (pgdp++, addr = next, addr != end);
}

static void map_kernel(pgd_t *pgdp)
{
    phys_addr_t pa;
    unsigned long va;
    unsigned long size;

    extern unsigned long __kimage_start[], __kimage_end[];
    pa = __pa_symbol(__kimage_start);
    va = (unsigned long)__kimage_start;
    size = (unsigned long)__kimage_end - (unsigned long)__kimage_start;

    create_pgd_mapping(pgdp, pa, va, size, PAGE_KERNEL_EXEC, early_pgtable_alloc, 0);
}

static void map_mem(pgd_t *pgdp)
{
    u64 i;
    phys_addr_t start, end;


    for_each_mem_range(i, &start, &end)
    {
        printk("start: [0x%x]\n", start);
        printk("end: [0x%x]\n", end);

        if(start >= end)
            break;

        create_pgd_mapping(pgdp, start, (unsigned long)phys_to_virt(start), end - start, PAGE_KERNEL, early_pgtable_alloc, 0);
    }
}

void paging_init(void)
{
    map_kernel(swapper_pg_dir);
    map_mem(swapper_pg_dir);

    create_pgd_mapping(swapper_pg_dir, 0x09000000, 0xFFFF100000000000, 0x1000, PAGE_DEVICE, early_pgtable_alloc, 0);

    cpu_replace_ttbr1(swapper_pg_dir);

    printk("hello new world page!\n");
}