#ifndef _ASM_PGTABLE_PROT_H
#define _ASM_PGTABLE_PROT_H

#include <const.h>
#include <asm/pgtable_hwdef.h>

/*
 * Software defined PTE bits definition.
 */
#define PTE_WRITE (PTE_DBM) /* same as DBM (51) */
#define PTE_DIRTY (_AT(pteval_t, 1) << 55)
#define PTE_SPECIAL (_AT(pteval_t, 1) << 56)
#define PTE_DEVMAP (_AT(pteval_t, 1) << 57)
#define PTE_PROT_NONE (_AT(pteval_t, 1) << 58) /* only when !PTE_VALID */

#define PROT_DEFAULT (PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define PROT_SECT_DEFAULT (PMD_TYPE_SECT | PMD_SECT_AF | PMD_SECT_S)

#define PROT_DEVICE_nGnRnE                                                     \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE |                        \
	 PTE_ATTRINDX(MT_DEVICE_nGnRnE))
#define PROT_DEVICE_nGnRE                                                      \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE |                        \
	 PTE_ATTRINDX(MT_DEVICE_nGnRE))
#define PROT_NORMAL_NC                                                         \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE |                        \
	 PTE_ATTRINDX(MT_NORMAL_NC))
#define PROT_NORMAL                                                            \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL))
#define PROT_NORMAL_TAGGED                                                     \
	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE |                        \
	 PTE_ATTRINDX(MT_NORMAL_TAGGED))

#define PROT_SECT_DEVICE_nGnRE                                                 \
	(PROT_SECT_DEFAULT | PMD_SECT_PXN | PMD_SECT_UXN |                     \
	 PMD_ATTRINDX(MT_DEVICE_nGnRE))
#define PROT_SECT_NORMAL                                                       \
	(PROT_SECT_DEFAULT | PMD_SECT_PXN | PMD_SECT_UXN |                     \
	 PMD_ATTRINDX(MT_NORMAL))
#define PROT_SECT_NORMAL_EXEC                                                  \
	(PROT_SECT_DEFAULT | PMD_SECT_UXN | PMD_ATTRINDX(MT_NORMAL))

#define PAGE_DEFAULT (PROT_DEFAULT | PTE_ATTRINDX(MT_NORMAL))

#define PAGE_KERNEL __pgprot(PROT_NORMAL)
#define PAGE_KERNEL_RO __pgprot((PROT_NORMAL & ~PTE_WRITE) | PTE_RDONLY)
#define PAGE_KERNEL_ROX                                                        \
	__pgprot((PROT_NORMAL & ~(PTE_WRITE | PTE_PXN)) | PTE_RDONLY)
#define PAGE_KERNEL_EXEC __pgprot(PROT_NORMAL & ~PTE_PXN)
#define PAGE_KERNEL_EXEC_CONT __pgprot((PROT_NORMAL & ~PTE_PXN) | PTE_CONT)

#define PAGE_DEVICE __pgprot(PROT_DEVICE_nGnRnE)

#endif