
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
	bl unexpected_exception

    restoreregister
	eret

IRQInterruptHandler:
	//进入中断，保存寄存器
	saveregister
	savefloatregister

	//执行中断处理函数
	bl handle_domain_irq

	//判断退出中断时候是否需要进行任务切换
    adr x2, task_thread_switch_interrupt_flag
    ldr x3, [x2]
	cmp x3, #1
	beq exit_irq_with_task_switch

	//不需要任务切换，恢复寄存器之后退出中断
	restorefloatregister
	restoreregister
	eret

//如果标志置位，需要在退出中断的时候进行任务切换
exit_irq_with_task_switch:
	//将中断切换标志位清零
    adr x2, task_thread_switch_interrupt_flag
	mov x3, #0
    str x3, [x2]

	//恢复寄存器，恢复到进入中断之前的状态
	restorefloatregister
	restoreregister

	//此时的状态是进入中断之前的状态，也就是from任务的状态

	//开始进行任务切换

	//保存from任务的状态到堆栈

    //1 保存from任务通用寄存器状态
    sub sp, sp, #30 * 8

    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    str x30, [sp, #16 * 15]

	//将from任务和to任务的任务信息恢复到x0、x1寄存器
    adr x2, task_interrupt_from_thread
    ldr x0, [x2]

    adr x2, task_interrupt_to_thread
    ldr x1, [x2]

    // 即将进行任务切换，更新to任务到当前任务信息 g_current_task
    //adr x2, g_current_task
    str x1, [x2]

    //2 保存from任务sp状态
    //获取当前任务sp状态
    //获取任务结构体中变量地址
    //保存sp地址到任务结构体中对应位置
    mov x3, sp
    mov x2, x0
    add x2, x2, #8 * 0
    str x3, [x2]

    //3 保存from任务cpcr状态
    //获取当前任务spsr状态
    //获取任务结构体中spsr变量位置
    //保存spsr到任务结构体中对应位置
    mrs x3, spsr_el1
    mov x2, x0
    add x2, x2, #8 * 1
    str x3, [x2]

    //4 保存from任务的返回地址
    //获取当前任务返回地址
    //获取任务结构体中变量地址
    //保存返回地址到任务结构体中对应位置
    mrs x3, elr_el1
    mov x2, x0
    add x2, x2, #8 * 2
    str x3, [x2]

	//开始从to任务堆栈恢复to任务
    //1 恢复to任务sp寄存器
    mov x2, x1
    add x2, x2, #8 * 0
    ldr x3, [x2]
    mov sp, x3

    //2 恢复to任务spsr状态寄存器
    mov x2, x1
    add x2, x2, #8 * 1
    ldr x3, [x2]
    msr spsr_el1, x3

    //3 恢复to任务异常返回地址
    mov x2, x1
    add x2, x2, #8 * 2
    ldr x3, [x2]
    msr elr_el1, x3

    //4 恢复to任务通用寄存器
    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15]

    //数据从堆栈取出之后，堆栈指针移动
    add sp, sp, #30 * 8

	//当前处理器状态已经切换到了to任务，异常返回之后到to任务执行
    eret


FIQInterruptHandler:
	saveregister

    bl unexpected_exception

    restoreregister
	eret

SErrorInterruptHandler:
	saveregister

	mov x0, sp
	bl unexpected_exception

	restoreregister
	eret

invalid_exception:
	b .
	saveregister

	mov x0, 1
	bl unexpected_exception

	restoreregister
	eret

    .type vectors, @function
    .size vectors, .-vectors
