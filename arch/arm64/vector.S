.macro save_context
    // push SPSR_EL1, ELR_EL1, x30–x0
    mrs     x0, SPSR_EL1
    str     x0, [sp, #-8]!
    mrs     x0, ELR_EL1
    str     x0, [sp, #-8]!
    str     x30, [sp, #-8]!
    str     x29, [sp, #-8]!
    str     x28, [sp, #-8]!
    str     x27, [sp, #-8]!
    str     x26, [sp, #-8]!
    str     x25, [sp, #-8]!
    str     x24, [sp, #-8]!
    str     x23, [sp, #-8]!
    str     x22, [sp, #-8]!
    str     x21, [sp, #-8]!
    str     x20, [sp, #-8]!
    str     x19, [sp, #-8]!
    str     x18, [sp, #-8]!
    str     x17, [sp, #-8]!
    str     x16, [sp, #-8]!
    str     x15, [sp, #-8]!
    str     x14, [sp, #-8]!
    str     x13, [sp, #-8]!
    str     x12, [sp, #-8]!
    str     x11, [sp, #-8]!
    str     x10, [sp, #-8]!
    str      x9, [sp, #-8]!
    str      x8, [sp, #-8]!
    str      x7, [sp, #-8]!
    str      x6, [sp, #-8]!
    str      x5, [sp, #-8]!
    str      x4, [sp, #-8]!
    str      x3, [sp, #-8]!
    str      x2, [sp, #-8]!
    str      x1, [sp, #-8]!
    str      x0, [sp, #-8]!
.endm

.macro restore_context
    // pop x0–x30, ELR_EL1, SPSR_EL1
    ldr      x0, [sp], #8
    msr     SPSR_EL1, x0
    ldr      x0, [sp], #8
    msr     ELR_EL1, x0
    ldr      x0, [sp], #8
    mov     x30, x0
    ldr     x29, [sp], #8
    ldr     x28, [sp], #8
    ldr     x27, [sp], #8
    ldr     x26, [sp], #8
    ldr     x25, [sp], #8
    ldr     x24, [sp], #8
    ldr     x23, [sp], #8
    ldr     x22, [sp], #8
    ldr     x21, [sp], #8
    ldr     x20, [sp], #8
    ldr     x19, [sp], #8
    ldr     x18, [sp], #8
    ldr     x17, [sp], #8
    ldr     x16, [sp], #8
    ldr     x15, [sp], #8
    ldr     x14, [sp], #8
    ldr     x13, [sp], #8
    ldr     x12, [sp], #8
    ldr     x11, [sp], #8
    ldr     x10, [sp], #8
    ldr      x9, [sp], #8
    ldr      x8, [sp], #8
    ldr      x7, [sp], #8
    ldr      x6, [sp], #8
    ldr      x5, [sp], #8
    ldr      x4, [sp], #8
    ldr      x3, [sp], #8
    ldr      x2, [sp], #8
    ldr      x1, [sp], #8
    ldr      x0, [sp], #8
.endm

.macro ventry label
    .align 7
    b \label
.endm

/*
 * Exception vectors.
 */
	.align	11
    .globl  vectors
vectors:
    // current el with sp0
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception

    // current el with spx
	ventry SynchronousInterruptHandler
	ventry IRQInterruptHandler
	ventry FIQInterruptHandler
	ventry SErrorInterruptHandler

    // lower el aarch64
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception

    // lower el aarch32
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception

SynchronousInterruptHandler:
    save_context

	bl unexpected_exception

    restore_context
    eret

IRQInterruptHandler:
    save_context

	bl handle_domain_irq

    restore_context
    eret

FIQInterruptHandler:
    save_context

    bl unexpected_exception

    restore_context
	eret

SErrorInterruptHandler:
    save_context

	bl unexpected_exception

    restore_context
	eret

invalid_exception:
    save_context

	bl unexpected_exception

    restore_context
	eret

    .type vectors, @function
    .size vectors, .-vectors
