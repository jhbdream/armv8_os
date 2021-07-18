#ifndef __LINUX_COMPILER_TYPES_H
#define __LINUX_COMPILER_TYPES_H

#define __iomem

#define __inline__ inline
#define __inline inline
#define noinline	__attribute__((noinline))

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif

#endif /* __LINUX_COMPILER_TYPES_H */
