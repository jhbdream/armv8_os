#include "aarch64_mmu.h"
#include "page_table.h"

/**
 *
 * 根据物理地址和虚拟地址创建页表映射
 *
 * pg_dir: pgd 页表基地址
 * phys: 需要映射的物理地址
 * virt: 需要映射的虚拟地址
 * size: 映射的地址长度
 * prot: 配置页表项的属性
 * pgtable_alloc: 如果没有页表，分配页表内存空间接口
 *
 */
static void create_pgd_mapping(pgd_t *pgdir, unsigned long phys,
			       unsigned long virt, unsigned long size,
			       pgprot_t prot,
			       unsigned long (*pgtable_alloc)(int))
{
}
