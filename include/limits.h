#ifndef __LIMITS_H__
#define __LIMITS_H__

#define USHRT_MAX	((unsigned short)~0U)
#define SHRT_MAX	((short)(USHRT_MAX >> 1))
#define SHRT_MAX	((short)(USHRT_MAX >> 1))
#define SHRT_MAX	((short)(USHRT_MAX >> 1))
#define INT_MAX		((int)(~0U >> 1))
#define INT32_MAX	((unsigned int)(~0U >> 1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define UINT32_MAX	(~0U)
#define LONG_MAX	((long)(~0UL >> 1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)
#define LLONG_MAX	((long long)(~0ULL >> 1))
#define LLONG_MIN	(-LLONG_MAX - 1)
#define ULLONG_MAX	(~0ULL)
#define UINTPTR_MAX	ULONG_MAX

#endif
