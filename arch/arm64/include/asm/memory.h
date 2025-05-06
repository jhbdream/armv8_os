/**
 * @file memory.h
 * @author jihongbin (longma@orbbec.com)
 * @brief
 *
 * @version 0.1
 * @date 2022-05-07
 * @copyright Copyright (c) {2015-2021} Orbbec
 *
 */

/**
 *
 *
 *	1. 定义虚拟地址空间划分
 *  2. 每个架构私有
 *  3. 包含 PAGE_OFFSET \ FIXADDR_TOP ...
 */

#ifndef __ASM_MEMORY_H
#define __ASM_MEMORY_H

#include <type.h>

#include <ee/sizes.h>

#define VA_BITS (48)

/* 物理内存基地址 */
#define PHYS_OFFSET (0x40000000)
#define PHYS_SIZE   (0x40000000)

#define PHYS_OFFSET (0x40000000)

/* 虚拟内存偏移地址 */
#define VIRT_OFFSET (0xFFFF000000000000)

/* 代码链接地址 */
#define KIMAGE_VADDR (0xFFFF000000080000)

/* 线性映射起始地址 */
#define PAGE_OFFSET (0xFFFF000100000000)
#define PAGE_END    (0xFFFF0001FFFFFFFF)

/* VMALLOC 虚拟地址 */
#define VMALLOC_START (0xFFFF000200000000)
#define VMALLOC_END   (0xFFFF0002FFFFFFFF)

/* DEVICE BASE */

#define DEVICE_PHYS_OFFSET (0x00000000)
#define DEVICE_PHYS_SIZE   (0x20000000)

#define DEVICE_START       (0xFFFF000300000000)
#define DEVICE_END         (0xFFFF0003FFFFFFFF)

/*
 * Memory types available.
 *
 * IMPORTANT: MT_NORMAL must be index 0 since vm_get_page_prot() may 'or' in
 *            the MT_NORMAL_TAGGED memory type for PROT_MTE mappings. Note
 *            that protection_map[] only contains MT_NORMAL attributes.
 */
#define MT_NORMAL        0
#define MT_NORMAL_TAGGED 1
#define MT_NORMAL_NC     2
#define MT_DEVICE_nGnRnE 3
#define MT_DEVICE_nGnRE  4

#ifndef __ASSEMBLY__

extern uint64_t kimage_voffset;

/*
 * Check whether an arbitrary address is within the linear map, which
 * lives in the [PAGE_OFFSET, PAGE_END) interval at the bottom of the
 * kernel's TTBR1 address range.
 */
#    define __is_lm_address(addr) (((u64)(addr)-PAGE_OFFSET) < (PAGE_END - PAGE_OFFSET))

#    define __lm_to_phys(addr)    (((addr)-PAGE_OFFSET) + PHYS_OFFSET)
#    define __kimg_to_phys(addr)  ((addr)-kimage_voffset)

#    define __phys_to_lm(x)       ((unsigned long)((x)-PHYS_OFFSET) | PAGE_OFFSET)
#    define __phys_to_kimg(x)     ((unsigned long)((x) + kimage_voffset))

#    define __virt_to_phys(x) \
        ({ __is_lm_address(x) ? __lm_to_phys(x) : __kimg_to_phys(x); })
#    define __phys_to_virt(x) (__phys_to_lm(x))

#    define virt_to_phys      virt_to_phys
static inline phys_addr_t virt_to_phys(const volatile void *x)
{
    return __virt_to_phys((unsigned long)(x));
}

#    define phys_to_virt phys_to_virt
static inline void *phys_to_virt(phys_addr_t x)
{
    return (void *)(__phys_to_virt(x));
}

#endif

#endif
