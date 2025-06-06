#ifndef __ALIGN_H__
#define __ALIGN_H__

#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define __ALIGN_KERNEL(x, a)         __ALIGN_KERNEL_MASK(x, (typeof(x))(a)-1)

/* @a is a power of 2 value */
#define ALIGN(x, a) __ALIGN_KERNEL((x), (a))

#endif
