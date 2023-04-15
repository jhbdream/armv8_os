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

#include <ee/sizes.h>
#include <config.h>
#include <type.h>

#define VA_BITS       (48)

/* 代码链接地址 */
#define KIMAGE_VADDR  (0xFFFF000000080000)

/* FIXMAP 起始地址 */
#define FIXADDR_SIZE  (SZ_512M)
#define FIXADDR_TOP   (KIMAGE_VADDR + FIXADDR_SIZE)

/* 线性映射起始地址? */
#define PAGE_OFFSET   (0xFFFF800000000000)

/* VMALLOC 虚拟地址 */
#define VMALLOC_START 0xFFFF900000000000

#endif
