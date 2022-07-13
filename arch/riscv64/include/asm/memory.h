#ifndef __ASM_MEMORY_H
#define __ASM_MEMORY_H

#include <config.h>
#include <ee/sizes.h>
#include <type.h>

#define VA_BITS (39)
#define _PAGE_OFFSET(va) (-(UL(1) << (va)))
#define PAGE_OFFSET (0xFFFF800000000000)
#define KIMAGE_VADDR (0xFFFF000000080000)
#define FIXADDR_TOP (KIMAGE_VADDR + SZ_512M)

#ifndef __ASSEMBLY__


#define __phys_to_virt(x) (x)
#define __virt_to_phys(x) (x)

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

#define __pa(x)     ((unsigned long)(x))
#define __pa_symbol(x) __phys_addr_symbol(x)

#endif /* #ifndef __ASSEMBLY__ */
#endif
