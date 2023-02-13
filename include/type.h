#ifndef __TYPE_H__
#define __TYPE_H__

#ifndef __ASSEMBLY__

#include <stdint.h>
#include <bitops.h>

#define DECLARE_BITMAP(name, bits) unsigned long name[BITS_TO_LONGS(bits)]

typedef int8_t __s8;
typedef uint8_t __u8;

typedef int16_t __s16;
typedef uint16_t __u16;

typedef int32_t __s32;
typedef uint32_t __u32;

typedef int64_t __s64;
typedef uint64_t __u64;

typedef __s8 s8;
typedef __u8 u8;
typedef __s16 s16;
typedef __u16 u16;
typedef __s32 s32;
typedef __u32 u32;
typedef __s64 s64;
typedef __u64 u64;

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#define __bitwise __bitwise__

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

typedef int bool;
typedef unsigned long uintptr_t;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

typedef u64 phys_addr_t;

struct list_head {
	struct list_head *next, *prev;
};

#endif /* #ifndef __ASSEMBLY__*/
#endif
