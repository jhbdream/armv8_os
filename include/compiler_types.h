#ifndef __LINUX_COMPILER_TYPES_H
#define __LINUX_COMPILER_TYPES_H

#define __iomem

# define likely(x)	__builtin_expect(!!(x), 1)
# define unlikely(x)	__builtin_expect(!!(x), 0)
# define likely_notrace(x)	likely(x)
# define unlikely_notrace(x)	unlikely(x)

#define __inline__ inline
#define __inline inline
#define noinline	__attribute__((noinline))

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif

/* Indirect macros required for expanded argument pasting, eg. __LINE__. */
#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)
#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#endif /* __LINUX_COMPILER_TYPES_H */
