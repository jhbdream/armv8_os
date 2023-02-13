#ifndef _ASM_RISCV_TLBFLUSH_H
#define _ASM_RISCV_TLBFLUSH_H

static inline void local_flush_tlb_all(void)
{
	__asm__ __volatile__("sfence.vma" : : : "memory");
}

/* Flush one page from local TLB */
static inline void local_flush_tlb_page(unsigned long addr)
{
	__asm__ __volatile__("sfence.vma %0" : : "r"(addr) : "memory");
}

#endif
