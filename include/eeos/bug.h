/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_BUG_H
#define _ASM_GENERIC_BUG_H

#define BUG() \
    do { \
        printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
        while (1) \
            ; \
    } while (0)

#define BUG_ON(condition) \
    do { \
        if ((condition)) \
            BUG(); \
    } while (0)

#endif /* _ASM_GENERIC_BUG_H */
