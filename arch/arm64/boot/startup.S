    .section StartUp, "ax"
    .balign 4

    .global _startup
    .type _startup, "function"

    /* default boot from el1 */
_startup:
	/* check CPU ID = 0x0, or jump to hang */
	mrs	x0, mpidr_el1
	and	x0, x0, #3
	cmp	x0, #0
	bne	hang

__init_el1:
#if 0
    mov   x0, INIT_SCTLR_EL1_MMU_OFF
    msr sctlr_el1, x0
    isb
#endif

    mov   x0, 0x3c5
    msr spsr_el1, x0
    adr x8, __init_el1_done
    msr elr_el1, x8
    eret

__init_el1_done:
__cpu_setup:
    /* Enable FP/SIMD at EL1
     * libc printk use float reg, if not enable fp will exception
     */
    mov x1, #3 << 20
    msr cpacr_el1, x1           // Enable FP/ASIMD

    // Zero the bss
    ldr x2, =__bss_start__ // Start of block
    ldr x3, =__bss_end__   // End of block

memset_loop:
    str wzr, [x2], #4
    cmp x2, x3
    b.lt memset_loop

    adr x8, __stack_top
    mov sp, x8


#if 0

    // 获取虚拟地址 跳转到虚拟地址执行
    adrp x0, __kimage_start
    ldr x8, =__mmu_on
    blr x8
__mmu_on:

    ldr x1, =__kimage_start
    sub x1, x1, x0
    //str x1, kimage_voffset, x5

    adrp   x8, vectors         // load VBAR_EL1 with virtual
    msr vbar_el1, x8            // vector table address
    isb

    adr x8, __stack_top
    mov sp, x8

#if __DEBUG_USE_GDB__ == 1
    b .
    nop
#endif

    // 创建临时的镜像映射 此时的页表是使用链接脚本分配的空间
    // 正式的页表需要后续使用动态分配内存创建映射关系、然后在切换页表
    bl start_kernel
#endif

hang:
	wfi
	b	hang
