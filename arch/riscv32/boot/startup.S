#include <asm/csr.h>

    .section StartUp, "ax"
    .balign 4
    .global _startup
    .type _startup, "function"

_startup:
	// 关闭全局中断
	csrw mie, 0
	// 清除全局中断状态
	csrw mip, 0

	fence.i

	li a0, -1
	csrw pmpaddr0, a0
	li a0, (PMP_A_NAPOT | PMP_R | PMP_W | PMP_X)
	csrw pmpcfg0, a0

	la a0, pmp_done
	csrw mtvec, a0

.align 2
pmp_done:
	/*
	 * Disable FPU to detect illegal usage of
	 * floating point in kernel space
	 */
	li t0, SR_FS
	csrc mstatus, t0

	//qemu run
    /* Clear BSS for flat non-ELF images */
    la a3, __bss_start
    la a4, __bss_stop
    ble a4, a3, clear_bss_done
clear_bss:
    sw zero, (a3)
    add a3, a3, 4
    blt a3, a4, clear_bss
clear_bss_done:
	la sp, __stack_top
	jal start_kernel

_exit:
	jal _exit

