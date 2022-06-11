/**
 * @file memory.h
 * @author jihongbin (longma@orbbec.com)
 * @brief
 * @version 0.1
 * @date 2022-05-07
 * @copyright Copyright (c) {2015-2021} Orbbec
 *
 */

#ifndef __ASM_MEMORY_H
#define __ASM_MEMORY_H

#include <config.h>
#include <ee/sizes.h>
#include <type.h>

#define VA_BITS (CONFIG_ARM64_VA_BITS)
#define KIMAGE_VADDR (0xFFFF000000080000)

#define FIXADDR_TOP (KIMAGE_VADDR + SZ_512M)


/*
 * Memory types available.
 *
 * IMPORTANT: MT_NORMAL must be index 0 since vm_get_page_prot() may 'or' in
 *        the MT_NORMAL_TAGGED memory type for PROT_MTE mappings. Note
 *        that protection_map[] only contains MT_NORMAL attributes.
 */
#define MT_NORMAL       0
#define MT_NORMAL_TAGGED    1
#define MT_NORMAL_NC        2
#define MT_DEVICE_nGnRnE    3
#define MT_DEVICE_nGnRE     4

#ifndef __ASSEMBLY__

extern uint64_t kimage_voffset;

#define virt_to_phys virt_to_phys
static inline phys_addr_t virt_to_phys(const volatile void *x)
{
    return ((unsigned long)(x));
}

#define phys_to_virt phys_to_virt
static inline void *phys_to_virt(phys_addr_t x)
{
    return (void *)((x));
}

#define __phys_to_kimg(x) ((unsigned long)((x) + kimage_voffset))
#define __kimg_to_phys(addr)    ((addr) - kimage_voffset)
#define __phys_addr_symbol(x) __kimg_to_phys((phys_addr_t)(x))

#define __pa(x)     ((unsigned long)(x))
#define __pa_symbol(x) __phys_addr_symbol(x)


#endif /* #ifndef __ASSEMBLY__ */
#endif