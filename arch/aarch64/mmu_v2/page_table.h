#ifndef __PAGE_TABLE_H__
#define __PAGE_TABLE_H__

#include <type.h>
#include <config.h>

/**
 * page table 相关定义 尽量实现通用
 * 默认只支持四级页表模式
 */

typedef u64 pgdval_t;
typedef u64 pudval_t;
typedef u64 pmdval_t;
typedef u64 pteval_t;
typedef u64 prot_val_t;

typedef struct {
	pgdval_t pgd;
} pgd_t;

typedef struct {
	pudval_t pud;
} pud_t;

typedef struct {
	pmdval_t pmd;
} pmd_t;

typedef struct {
	pteval_t pte;
} pte_t;

typedef struct {
	prot_val_t pgprot;
} pgprot_t;

#define pgd_val(x) ((x).pgd)
#define _pgd(x) ((pgd_t){ (x) })

#define pud_val(x) ((x).pud)
#define _pud(x) ((pud_t){ (x) })

#define pmd_val(x) ((x).pmd)
#define _pmd(x) ((pmd_t){ (x) })

#define pte_val(x) ((x).pte)
#define _pte(x) ((pte_t){ (x) })

#define pgprot_val(x) ((x).pgprot)
#define _pgprot(x) ((pgprot_t){ (x) })

#endif
