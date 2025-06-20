#ifndef __ASM_ARM_IO_H
#define __ASM_ARM_IO_H

/*
 * Generic IO read/write.  These perform native-endian accesses.
 */
#define __raw_writeb __raw_writeb
static inline void __raw_writeb(uint8_t val, volatile void *addr)
{
    asm volatile("strb %w0, [%1]" : : "rZ"(val), "r"(addr));
}

#define __raw_writew __raw_writew
static inline void __raw_writew(uint16_t val, volatile void *addr)
{
    asm volatile("strh %w0, [%1]" : : "rZ"(val), "r"(addr));
}

#define __raw_writel __raw_writel
static inline void __raw_writel(uint32_t val, volatile void *addr)
{
    asm volatile("str %w0, [%1]" : : "rZ"(val), "r"(addr));
}

#define __raw_writeq __raw_writeq
static inline void __raw_writeq(uint64_t val, volatile void *addr)
{
    asm volatile("str %x0, [%1]" : : "rZ"(val), "r"(addr));
}

#define __raw_readb __raw_readb
static inline uint8_t __raw_readb(const volatile void *addr)
{
    uint8_t val;
    asm volatile("ldrb %w0, [%1]" : "=r"(val) : "r"(addr));
    return val;
}

#define __raw_readw __raw_readw
static inline uint16_t __raw_readw(const volatile void *addr)
{
    uint16_t val;

    asm volatile("ldrh %w0, [%1]" : "=r"(val) : "r"(addr));
    return val;
}

#define __raw_readl __raw_readl
static inline uint32_t __raw_readl(const volatile void *addr)
{
    uint32_t val;
    asm volatile("ldr %w0, [%1]" : "=r"(val) : "r"(addr));
    return val;
}

#define __raw_readq __raw_readq
static inline uint64_t __raw_readq(const volatile void *addr)
{
    uint64_t val;
    asm volatile("ldr %0, [%1]" : "=r"(val) : "r"(addr));
    return val;
}

/* IO barriers */
/*								                                \
 * Create a dummy control dependency from the IO read to any	    \
 * later instructions. This ensures that a subsequent call to	\
 * udelay() will be ordered due to the ISB in get_cycles().	    \
 */

#define __iormb(v) \
    ({ \
        unsigned long tmp; \
        asm volatile("eor	%0, %1, %1\n" \
                     "cbnz	%0, ." \
                     : "=r"(tmp) \
                     : "r"((unsigned long)(v)) \
                     : "memory"); \
    })

#define isb()       asm volatile("isb" : : : "memory")
#define dmb(opt)    asm volatile("dmb " #opt : : : "memory")
#define dsb(opt)    asm volatile("dsb " #opt : : : "memory")

#define __io_par(v) __iormb(v)
#define __iowmb()   dmb(oshst)
#define __iomb()    dmb(osh)

/*
 * Relaxed I/O memory access primitives. These follow the Device memory
 * ordering rules but do not guarantee any ordering relative to Normal memory
 * accesses.
 */
#define readb_relaxed(c) \
    ({ \
        uint8_t __r = __raw_readb(c); \
        __r; \
    })
#define readw_relaxed(c) \
    ({ \
        uint16_t __r = __raw_readw(c); \
        __r; \
    })
#define readl_relaxed(c) \
    ({ \
        uint32_t __r = __raw_readl(c); \
        __r; \
    })
#define readq_relaxed(c) \
    ({ \
        uint64_t __r = __raw_readq(c); \
        __r; \
    })

#define writeb_relaxed(v, c) ((void)__raw_writeb((v), (c)))
#define writew_relaxed(v, c) ((void)__raw_writew((v), (c)))
#define writel_relaxed(v, c) ((void)__raw_writel((v), (c)))
#define writeq_relaxed(v, c) ((void)__raw_writeq((v), (c)))

/*
 * I/O memory access primitives. Reads are ordered relative to any
 * following Normal memory access. Writes are ordered relative to any prior
 * Normal memory access.
 */
#define readb(c) \
    ({ \
        uint8_t __v = readb_relaxed(c); \
        __iormb(__v); \
        __v; \
    })
#define readw(c) \
    ({ \
        uint16_t __v = readw_relaxed(c); \
        __iormb(__v); \
        __v; \
    })
#define readl(c) \
    ({ \
        uint32_t __v = readl_relaxed(c); \
        __iormb(__v); \
        __v; \
    })
#define readq(c) \
    ({ \
        uint64_t __v = readq_relaxed(c); \
        __iormb(__v); \
        __v; \
    })

#define writeb(v, c) \
    ({ \
        __iowmb(); \
        writeb_relaxed((v), (c)); \
    })
#define writew(v, c) \
    ({ \
        __iowmb(); \
        writew_relaxed((v), (c)); \
    })
#define writel(v, c) \
    ({ \
        __iowmb(); \
        writel_relaxed((v), (c)); \
    })
#define writeq(v, c) \
    ({ \
        __iowmb(); \
        writeq_relaxed((v), (c)); \
    })

#endif /* __ASM_ARM_IO_H */
