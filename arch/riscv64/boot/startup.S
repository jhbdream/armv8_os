#include <asm/csr.h>
#include <asm/memory.h>
#include <asm/pgtable.h>

    .section StartUp, "ax"
    .balign 4
    .global _startup
    .type _startup, "function"

_startup:

_machine_mode:
	csrw mie, 0
	csrw mip, 0
	fence.i

	/* trap all interrupt && exception to s-mode */
	li a0, 0xFFFFFFFFFFFFFFFF
	csrw mideleg, a0
	csrw medeleg, a0

	/* riscv pmp need to set before exec mret */
	/*
	 * Setup a PMP to permit access to all of memory.  Some machines may
	 * not implement PMPs, so we set up a quick trap handler to just skip
	 * touching the PMPs on any trap.
	 */
	la a0, pmp_done
	csrw mtvec, a0

	li a0, -1
	csrw pmpaddr0, a0
	li a0, (PMP_A_NAPOT | PMP_R | PMP_W | PMP_X)
	csrw pmpcfg0, a0
.align 2
pmp_done:

	/* config swtich to super mode */
	csrr a0, mstatus

	/* set mpp = 1 */
	li a1, ~(0x3 << 11)
	and a0, a0, a1
	li a1, (0x1 << 11)
	or a0, a0, a1

	/* clear mpie */
	li a1, ~(0x1 << 7)
	and a0, a0, a1

	csrw mstatus, a0

	la a0, __supervisor_mode
	csrw mepc, a0
	mret

__supervisor_mode:
	csrw sie, 0
	csrw sip, 0

	/*
	 * Disable FPU to detect illegal usage of
	 * floating point in kernel space
	 */
	li t0, SR_FS
	csrc sstatus, t0

	la t0, handle_exception
	csrw stvec, t0

    /* Clear BSS for flat non-ELF images */
    la a3, __bss_start
    la a4, __bss_stop
    ble a4, a3, clear_bss_done
clear_bss:
    sd zero, (a3)
    add a3, a3, 8
    blt a3, a4, clear_bss
clear_bss_done:

	la sp, __stack_top

	call setup_vm

begin_config_mmu:

	la a0, __kimage_start
	li a1, KIMAGE_VADDR
	sub a1, a1, a0

	/* 设置异常入口地址为 enable_mmu 虚拟地址 */
	la a0, enable_mmu
	add a0, a0, a1
	csrw stvec, a0

	/* config satp */
	la a1, early_pg_dir
	srl a0, a1, PAGE_SHIFT

	/* sv39 page mode */
	li a1, (0x8 << 60)
	or a0, a1, a0

	//同步页表更新
	sfence.vma

	/* 开启mmu 触发异常,跳转到 stvec  */
	csrw satp, a0

enable_mmu:
	/* begin run in virt address space */
	la sp, __stack_top

	jal .

	jal start_kernel

_exit:
	jal _exit

