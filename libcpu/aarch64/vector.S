
.macro saveregister
	stp		X0,X1, [sp,#-0x10]!
	stp		X2,X3, [sp,#-0x10]!
	stp		X4,X5, [sp,#-0x10]!
	stp		X6,X7, [sp,#-0x10]!
	stp		X8,X9, [sp,#-0x10]!
	stp		X10,X11, [sp,#-0x10]!
	stp		X12,X13, [sp,#-0x10]!
	stp		X14,X15, [sp,#-0x10]!
	stp		X16,X17, [sp,#-0x10]!
	stp 	X18,X19, [sp,#-0x10]!
	stp 	X29,X30, [sp,#-0x10]!

	mrs x0, CPACR_EL1
	mrs x1, ELR_EL1
	mrs	x2, SPSR_EL1
	stp	x0, x1, [sp,#-0x10]!
	str	x2, [sp,#-0x10]!
.endm

.macro restoreregister
	ldr	x2,[sp],0x10
	ldp	x0, x1, [sp],0x10
	msr	CPACR_EL1, x0
	msr	ELR_EL1, x1
	msr	SPSR_EL1, x2

	ldp 	X29,X30, [sp], #0x10
	ldp 	X18,X19, [sp], #0x10
	ldp		X16,X17, [sp], #0x10
	ldp		X14,X15, [sp], #0x10
	ldp		X12,X13, [sp], #0x10
	ldp		X10,X11, [sp], #0x10
	ldp		X8,X9, [sp], #0x10
	ldp		X6,X7, [sp], #0x10
	ldp		X4,X5, [sp], #0x10
	ldp		X2,X3, [sp], #0x10
	ldp		X0,X1, [sp], #0x10
.endm

.macro savefloatregister

/* Save all the floating point register to the stack */
	stp	q0,q1, [sp, #-0x20]!
	stp	q2,q3, [sp, #-0x20]!
	stp	q4,q5, [sp, #-0x20]!
	stp	q6,q7, [sp, #-0x20]!
	stp	q8,q9, [sp, #-0x20]!
	stp	q10,q11, [sp, #-0x20]!
	stp	q12,q13, [sp, #-0x20]!
	stp	q14,q15, [sp, #-0x20]!
	stp	q16,q17, [sp, #-0x20]!
	stp	q18,q19, [sp, #-0x20]!
	stp	q20,q21, [sp, #-0x20]!
	stp	q22,q23, [sp, #-0x20]!
	stp	q24,q25, [sp, #-0x20]!
	stp	q26,q27, [sp, #-0x20]!
	stp	q28,q29, [sp, #-0x20]!
	stp	q30,q31, [sp, #-0x20]!

	mrs	x2, FPCR
	mrs	x3, FPSR
	stp	x2, x3, [sp, #-0x10]!
.endm

.macro restorefloatregister

/* Restore all the floating point register from the stack */
	ldp	x2, x3, [sp] ,#0x10
	msr	FPCR, x2
	msr	FPSR, x3

	ldp	q30,q31, [sp] ,#0x20
	ldp	q28,q29, [sp] ,#0x20
	ldp	q26,q27, [sp] ,#0x20
	ldp	q24,q25, [sp] ,#0x20
	ldp	q22,q23, [sp] ,#0x20
	ldp	q20,q21, [sp] ,#0x20
	ldp	q18,q19, [sp] ,#0x20
	ldp	q16,q17, [sp] ,#0x20
	ldp	q14,q15, [sp] ,#0x20
	ldp	q12,q13, [sp] ,#0x20
	ldp	q10,q11, [sp] ,#0x20
	ldp	q8,q9, [sp] ,#0x20
	ldp	q6,q7, [sp] ,#0x20
	ldp	q4,q5, [sp] ,#0x20
	ldp	q2,q3, [sp] ,#0x20
	ldp	q0,q1, [sp] ,#0x20
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
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception

	ventry SynchronousInterruptHandler
	ventry IRQInterruptHandler
	ventry FIQInterruptHandler
	ventry SErrorInterruptHandler

	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception

	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception
	ventry invalid_exception

SynchronousInterruptHandler:
	saveregister

	mov x0, 2
	bl	irq_test

    restoreregister
	eret

IRQInterruptHandler:
	saveregister
	savefloatregister

	bl handle_domain_irq

	restorefloatregister
	restoreregister
	eret

FIQInterruptHandler:
	saveregister

    bl irq_test

    restoreregister
	eret

SErrorInterruptHandler:
	saveregister

	mov x0, sp
	bl irq_test

	restoreregister
	eret

invalid_exception:
	saveregister

	mov x0, 1
	bl irq_test

	restoreregister
	eret

    .type vectors, @function
    .size vectors, .-vectors