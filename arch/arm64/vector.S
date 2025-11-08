// 栈空间按 16 字节对齐，偏移固定
// SP 偏移：x0-x30 (31*8 = 248), SP=248, ELR_EL1=256, SPSR_EL1=264

.macro save_context
    sub sp, sp, #(31*8 + 8 + 8 + 8)   // 分配 280 字节栈空间

    // 保存通用寄存器 x0-x30
    stp x0,  x1,  [sp, 0]
    stp x2,  x3,  [sp, 16]
    stp x4,  x5,  [sp, 32]
    stp x6,  x7,  [sp, 48]
    stp x8,  x9,  [sp, 64]
    stp x10, x11, [sp, 80]
    stp x12, x13, [sp, 96]
    stp x14, x15, [sp, 112]
    stp x16, x17, [sp, 128]
    stp x18, x19, [sp, 144]
    stp x20, x21, [sp, 160]
    stp x22, x23, [sp, 176]
    stp x24, x25, [sp, 192]
    stp x26, x27, [sp, 208]
    stp x28, x29, [sp, 224]
    str x30, [sp, 240]          // LR

    // 保存原 SP
    str sp, [sp, 248]

    // 保存 ELR_EL1 / SPSR_EL1
    mrs x0, ELR_EL1
    str x0, [sp, 256]
    mrs x0, SPSR_EL1
    str x0, [sp, 264]
.endm

.macro restore_context
    // 恢复系统寄存器
    ldr x0, [sp, 264]
    msr SPSR_EL1, x0
    ldr x0, [sp, 256]
    msr ELR_EL1, x0

    // 恢复通用寄存器 x0-x30
    ldr x30, [sp, 240]
    ldp x28, x29, [sp, 224]
    ldp x26, x27, [sp, 208]
    ldp x24, x25, [sp, 192]
    ldp x22, x23, [sp, 176]
    ldp x20, x21, [sp, 160]
    ldp x18, x19, [sp, 144]
    ldp x16, x17, [sp, 128]
    ldp x14, x15, [sp, 112]
    ldp x12, x13, [sp, 96]
    ldp x10, x11, [sp, 80]
    ldp x8, x9, [sp, 64]
    ldp x6, x7, [sp, 48]
    ldp x4, x5, [sp, 32]
    ldp x2, x3, [sp, 16]
    ldp x0, x1, [sp, 0]
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


IRQInterruptHandler:
    save_context

	bl handle_domain_irq

    restore_context

    // 处理任务调度的内容 当前是 current thread 的状态
    // 判断是否需要进行上下文切换
    // 如果需要切换则保存状态到当前任务栈
    // 从目标任务栈恢复状态

    // 结束中断处理
    eret

SynchronousInterruptHandler:
    mov x0, 'A'
    bl unexpected_exception

FIQInterruptHandler:
    mov x0, 'B'
    bl unexpected_exception

SErrorInterruptHandler:
    mov x0, 'C'
    bl unexpected_exception

invalid_exception:
    mov x0, 'D'
    bl unexpected_exception

    .type vectors, @function
    .size vectors, .-vectors
