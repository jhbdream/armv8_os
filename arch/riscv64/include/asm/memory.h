#ifndef __ASM_MEMORY_H
#define __ASM_MEMORY_H

#include <config.h>
#include <ee/sizes.h>
#include <type.h>

#define VA_BITS (39)
#define _PAGE_OFFSET(va) (-(UL(1) << (va - 1)))

/* phys memory start */
#define PHYS_OFFSET (memory_start)

/*riscv mmu bit[64:39] need to same to bit[38]*/
/* +256G */
#define PAGE_OFFSET (0xFFFFFFE000000000)
/* +0G */
#define KIMAGE_VADDR (0xFFFFFFC000000000)

/* FIXADDR_TOP 和 KIMAGE_VADDR 设置为不同的PMD */
#define FIXADDR_TOP (KIMAGE_VADDR + SZ_2G)

#define FIXADDR_SIZE PMD_SIZE
#define FIXADDR_START (FIXADDR_TOP - FIXADDR_SIZE)

#define PHYS_PFN_OFFSET (PHYS_OFFSET >> PAGE_SHIFT)
#define ARCH_PFN_OFFSET ((unsigned long)PHYS_PFN_OFFSET)

#ifndef __ASSEMBLY__

/* mm page_va - mm phys_pa */
extern unsigned long va_pa_offset;

/* kernel_image_start_va - load_pa */
extern unsigned long kimage_voffset;

extern signed long memory_start;

#define __phys_to_kimg(x) ((unsigned long)((x) + kimage_voffset))
#define __kimg_to_phys(addr) ((addr)-kimage_voffset)
#define __phys_addr_symbol(x) __kimg_to_phys((phys_addr_t)(x))

#define __phys_to_virt(x) ((x - PHYS_OFFSET) | PAGE_OFFSET)
#define __virt_to_phys(x)                                                      \
	(__is_lm_address(x) ? __lm_to_phys(x) : __kimg_to_phys(x))

#define virt_to_phys virt_to_phys
static inline phys_addr_t virt_to_phys(const volatile void *x)
{
	return (__virt_to_phys((unsigned long)(x)));
}

#define phys_to_virt phys_to_virt
static inline void *phys_to_virt(phys_addr_t x)
{
	return (void *)(__phys_to_virt(x));
}

#define __pa(x) virt_to_phys(x)
#define __pa_symbol(x) __phys_addr_symbol(x)
#define __lm_to_phys(addr) (((addr)-PAGE_OFFSET) + PHYS_OFFSET)
#define __is_lm_address(addr) (((u64)(addr) >= PAGE_OFFSET))

#define __va(x) phys_to_virt(x)

#endif /* #ifndef __ASSEMBLY__ */
#endif
