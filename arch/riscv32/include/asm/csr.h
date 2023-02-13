#ifndef _ASM_RISCV_CSR_H
#define _ASM_RISCV_CSR_H

#include <const.h>
#include <asm/asm.h>

#define SR_FS _AC(0x00006000, UL) /* Floating-point Status */

/* PMP configuration */
#define PMP_R 0x01
#define PMP_W 0x02
#define PMP_X 0x04
#define PMP_A 0x18
#define PMP_A_TOR 0x08
#define PMP_A_NA4 0x10
#define PMP_A_NAPOT 0x18
#define PMP_L 0x80

#ifndef __ASSEMBLY__

#define csr_swap(csr, val)                                                     \
	({                                                                     \
		unsigned long __v = (unsigned long)(val);                      \
		__asm__ __volatile__("csrrw %0, " __ASM_STR(csr) ", %1"        \
				     : "=r"(__v)                               \
				     : "rK"(__v)                               \
				     : "memory");                              \
		__v;                                                           \
	})

#define csr_read(csr)                                                          \
	({                                                                     \
		register unsigned long __v;                                    \
		__asm__ __volatile__("csrr %0, " __ASM_STR(csr)                \
				     : "=r"(__v)                               \
				     :                                         \
				     : "memory");                              \
		__v;                                                           \
	})

#define csr_write(csr, val)                                                    \
	({                                                                     \
		unsigned long __v = (unsigned long)(val);                      \
		__asm__ __volatile__("csrw " __ASM_STR(csr) ", %0"             \
				     :                                         \
				     : "rK"(__v)                               \
				     : "memory");                              \
	})

#define csr_read_set(csr, val)                                                 \
	({                                                                     \
		unsigned long __v = (unsigned long)(val);                      \
		__asm__ __volatile__("csrrs %0, " __ASM_STR(csr) ", %1"        \
				     : "=r"(__v)                               \
				     : "rK"(__v)                               \
				     : "memory");                              \
		__v;                                                           \
	})

#define csr_set(csr, val)                                                      \
	({                                                                     \
		unsigned long __v = (unsigned long)(val);                      \
		__asm__ __volatile__("csrs " __ASM_STR(csr) ", %0"             \
				     :                                         \
				     : "rK"(__v)                               \
				     : "memory");                              \
	})

#define csr_read_clear(csr, val)                                               \
	({                                                                     \
		unsigned long __v = (unsigned long)(val);                      \
		__asm__ __volatile__("csrrc %0, " __ASM_STR(csr) ", %1"        \
				     : "=r"(__v)                               \
				     : "rK"(__v)                               \
				     : "memory");                              \
		__v;                                                           \
	})

#define csr_clear(csr, val)                                                    \
	({                                                                     \
		unsigned long __v = (unsigned long)(val);                      \
		__asm__ __volatile__("csrc " __ASM_STR(csr) ", %0"             \
				     :                                         \
				     : "rK"(__v)                               \
				     : "memory");                              \
	})

#endif /* __ASSEMBLY__ */

#endif
