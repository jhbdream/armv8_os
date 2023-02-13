#ifndef __LINUX_COMPILER_ATTRIBUTES_H
#define __LINUX_COMPILER_ATTRIBUTES_H

#define __attribute_const__ __attribute__((__const__))
#define __aligned(x) __attribute__((__aligned__(x)))
#define __section(section) __attribute__((__section__(section)))

#endif
