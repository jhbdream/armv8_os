#include <linkage.h>
#include <asm/asm.h>

.macro save_register
	addi sp, sp, -34 * REGBYTES

	sd x0,   0 * REGBYTES(sp)
	sd x1,   1 * REGBYTES(sp)
	sd x2,   2 * REGBYTES(sp)
	sd x3,   3 * REGBYTES(sp)
	sd x4,   4 * REGBYTES(sp)
	sd x5,   5 * REGBYTES(sp)
	sd x6,   6 * REGBYTES(sp)
	sd x7,   7 * REGBYTES(sp)
	sd x8,   8 * REGBYTES(sp)
	sd x9,   9 * REGBYTES(sp)
	sd x10, 10 * REGBYTES(sp)
	sd x11, 11 * REGBYTES(sp)
	sd x12, 12 * REGBYTES(sp)
	sd x13, 13 * REGBYTES(sp)
	sd x14, 14 * REGBYTES(sp)
	sd x15, 15 * REGBYTES(sp)
	sd x16, 16 * REGBYTES(sp)
	sd x17, 17 * REGBYTES(sp)
	sd x18, 18 * REGBYTES(sp)
	sd x19, 19 * REGBYTES(sp)
	sd x20, 20 * REGBYTES(sp)
	sd x21, 21 * REGBYTES(sp)
	sd x22, 22 * REGBYTES(sp)
	sd x23, 23 * REGBYTES(sp)
	sd x24, 24 * REGBYTES(sp)
	sd x25, 25 * REGBYTES(sp)
	sd x26, 26 * REGBYTES(sp)
	sd x27, 27 * REGBYTES(sp)
	sd x28, 28 * REGBYTES(sp)
	sd x29, 29 * REGBYTES(sp)
	sd x30, 30 * REGBYTES(sp)
	sd x31, 31 * REGBYTES(sp)

	csrr t0, mstatus
	sd t0, 32 * REGBYTES(sp)

	csrr t0, mepc
	sd t0, 33 * REGBYTES(sp)
.endm

.macro restore_register
	ld t0, 32 * REGBYTES(sp)
	csrw mstatus, t0

	ld t0, 33 * REGBYTES(sp)
	csrw mepc, t0

	ld x0,   0 * REGBYTES(sp)
	ld x1,   1 * REGBYTES(sp)
	ld x2,   2 * REGBYTES(sp)
	ld x3,   3 * REGBYTES(sp)
	ld x4,   4 * REGBYTES(sp)
	ld x5,   5 * REGBYTES(sp)
	ld x6,   6 * REGBYTES(sp)
	ld x7,   7 * REGBYTES(sp)
	ld x8,   8 * REGBYTES(sp)
	ld x9,   9 * REGBYTES(sp)
	ld x10, 10 * REGBYTES(sp)
	ld x11, 11 * REGBYTES(sp)
	ld x12, 12 * REGBYTES(sp)
	ld x13, 13 * REGBYTES(sp)
	ld x14, 14 * REGBYTES(sp)
	ld x15, 15 * REGBYTES(sp)
	ld x16, 16 * REGBYTES(sp)
	ld x17, 17 * REGBYTES(sp)
	ld x18, 18 * REGBYTES(sp)
	ld x19, 19 * REGBYTES(sp)
	ld x20, 20 * REGBYTES(sp)
	ld x21, 21 * REGBYTES(sp)
	ld x22, 22 * REGBYTES(sp)
	ld x23, 23 * REGBYTES(sp)
	ld x24, 24 * REGBYTES(sp)
	ld x25, 25 * REGBYTES(sp)
	ld x26, 26 * REGBYTES(sp)
	ld x27, 27 * REGBYTES(sp)
	ld x28, 28 * REGBYTES(sp)
	ld x29, 29 * REGBYTES(sp)
	ld x30, 30 * REGBYTES(sp)
	ld x31, 31 * REGBYTES(sp)

	addi sp, sp, 34 * REGBYTES
.endm

.global trap_handler

ENTRY(handle_exception)
	save_register

	/* call c trap handler */
	csrr a0, mcause
	csrr a1, mtval
	call trap_handler

	restore_register
	mret
END(handle_exception)
