    .section StartUp, "ax"
    .balign 4

    .global _startup
    .type _startup, "function"

_startup:
	/* check CPU ID = 0x0, or jump to hang */
	mrs	x0, mpidr_el1
	and	x0, x0, #3
	cmp	x0, #0
	bne	hang

    /* Enable FP/SIMD at EL1
     * libc printf use float reg, if not enable fp will exception
     */
    mov x0, #(3<<20)
    msr cpacr_el1, x0

    ldr x0, =__stack_top
    mov sp, x0

    // Zero the bss
    ldr x0, =__bss_start__ // Start of block
    ldr x1, =__bss_end__   // End of block

memset_loop:
    str wzr, [x0], #4
    cmp x0, x1
    b.lt memset_loop

    bl main

    b exit // Will not return

hang:
	wfi
	b	hang