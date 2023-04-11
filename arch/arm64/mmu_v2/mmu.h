#ifndef __PAGE_TABLE_H__
#define __PAGE_TABLE_H__

#include "pgtable.h"
#include "config.h"
#include "type.h"

/* arm64 mmu 页表硬件定义 */
#define PTE_TYPE_PAGE (_AT(pteval_t, 3) << 0)
#define PTE_AF (_AT(pteval_t, 1) << 10) /* Access Flag */
#define PTE_SHARED (_AT(pteval_t, 3) << 8) /* SH[1:0], inner shareable */
#define PTE_PXN (_AT(pteval_t, 1) << 53) /* Privileged XN */
#define PTE_UXN (_AT(pteval_t, 1) << 54) /* User XN */
#define PTE_WRITE (_AT(pteval_t, 1) << 51) /* same as DBM (51) */

#define MT_NORMAL 0
#define MT_NORMAL_TAGGED 1
#define MT_NORMAL_NC 2
#define MT_DEVICE_nGnRnE 3
#define MT_DEVICE_nGnRE 4
#define PTE_ATTRINDX(t) (_AT(pteval_t, (t)) << 2)

#define PROT_DEFAULT (PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define PROT_NORMAL                                                            \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL))

#define PROT_DEVICE_nGnRnE                                                     \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE |                        \
	 PTE_ATTRINDX(MT_DEVICE_nGnRnE))

#define PAGE_TYPE_TABLE (0x3)
#define PAGE_TYPE_BLOCK (0)

#endif
