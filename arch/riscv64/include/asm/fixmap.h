#ifndef _ASM_RISCV_FIXMAP_H
#define _ASM_RISCV_FIXMAP_H

#include <asm/pgtable.h>
#include <ee/sizes.h>

/*
 * Here we define all the compile-time 'special' virtual addresses.
 * The point is to have a constant address at compile time, but to
 * set the physical address only in the boot process.
 *
 * These 'compile-time allocated' memory buffers are page-sized. Use
 * set_fixmap(idx,phys) to associate physical memory with fixmap indices.
 */
enum fixed_addresses {
	FIX_HOLE,
#define FIX_FDT_SIZE	SZ_1M
	FIX_FDT_END,
	FIX_FDT = FIX_FDT_END + FIX_FDT_SIZE / PAGE_SIZE - 1,
	FIX_PTE,
	FIX_PMD,
	FIX_TEXT_POKE1,
	FIX_TEXT_POKE0,
	FIX_EARLYCON_MEM_BASE,
	__end_of_fixed_addresses
};

#include <asm-generic/fixmap.h>

#endif /* _ASM_RISCV_FIXMAP_H */
