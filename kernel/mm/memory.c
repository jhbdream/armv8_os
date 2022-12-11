#include "asm/page-def.h"
#include "asm/pgalloc.h"
#include "asm/pgtable.h"
#include "asm/pgtable_hwdef.h"
#include "asm/pgtable_type.h"
#include "mm/page_alloc.h"
#include "type.h"
#include <ee/mm.h>
#include <pgtable.h>
#include <string.h>
#include <errno.h>

int __pud_alloc(pgd_t *pgd, unsigned long address)
{
	pud_t *new = __get_free_page();
	if(!new)
		return -ENOMEM;

	/* 为分配页表项时 页表项的内容需要为空,表示未进行映射
	 * 等待下一级别的页表进行填充
	 */
	memset(new, 0, PAGE_SIZE);

	/* 可能会出现共享资源访问？？？ */
	// TODO: 不同硬件架构通用处理
	// TODO: spin lock

	/**
	 * check pgd presend 检查pgd是否有效
	 * 如果 pgd 有效则不需要分配下一级的pud
	 * 如果 pgd 无效则填充新分配的pmd
	 *
	 */
	if(pgd_present(*pgd))
		free_page((unsigned long)new);
	else
		pgd_populate(pgd, new, PGD_TYPE_TABLE);

	// TODO: spin unlock

	return 0;
}

int __pmd_alloc(pud_t *pud, unsigned long address)
{
	pmd_t *new = __get_free_page();
	if(!new)
		return -ENOMEM;

	memset(new, 0, PAGE_SIZE);

	// TODO: spin lock

	if(pud_present(*pud))
		free_page((unsigned long)new);
	else
		pud_populate(pud, new, PGD_TYPE_TABLE);

	// TODO: spin unlock

	return 0;
}

int __pte_alloc(pmd_t *pmd, unsigned long address)
{
	pte_t *new = __get_free_page();
	if(!new)
		return -ENOMEM;

	memset(new, 0, PAGE_SIZE);

	if(pmd_present(*pmd))
		free_page((unsigned long)new);
	else
		pmd_populate(pmd, new, PGD_TYPE_TABLE);

	return 0;
}

