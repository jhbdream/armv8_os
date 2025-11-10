.macro save_context
    // frame layout (bytes, offsets from new SP):
    // 0x00  - x0
    // 0x08  - x1
    // 0x10  - x2
    // ...
    // 0xE0  - x30  (offset 240)
    // 0xF8  - SPSR_EL1 (offset 248)
    // 0x100 - ELR_EL1  (offset 256)
    // total used = 264 -> round up to 272 for 16-byte alignment

    // 常量：帧大小（16 字节对齐）
    .equ FRAME_SIZE, 272        // 0x110

    // allocate frame
    sub     sp, sp, #FRAME_SIZE

    // 保存通用寄存器 x0..x29 （15 对，每对占 16 字节）
    stp     x0,  x1,  [sp, #0]       // x0 @0x00, x1 @0x08
    stp     x2,  x3,  [sp, #16]      // x2 @0x10, x3 @0x18
    stp     x4,  x5,  [sp, #32]
    stp     x6,  x7,  [sp, #48]
    stp     x8,  x9,  [sp, #64]
    stp     x10, x11, [sp, #80]
    stp     x12, x13, [sp, #96]
    stp     x14, x15, [sp, #112]
    stp     x16, x17, [sp, #128]
    stp     x18, x19, [sp, #144]
    stp     x20, x21, [sp, #160]
    stp     x22, x23, [sp, #176]
    stp     x24, x25, [sp, #192]
    stp     x26, x27, [sp, #208]
    stp     x28, x29, [sp, #224]

    // 单独保存 x30 (LR)
    str     x30, [sp, #240]          // x30 @0xF0 (240)

    // 保存 SPSR_EL1 和 ELR_EL1 到栈（使用 x17 作临时）
    mrs     x17, spsr_el1
    str     x17, [sp, #248]          // SPSR_EL1 @0xF8 (248)

    mrs     x17, elr_el1
    str     x17, [sp, #256]          // ELR_EL1  @0x100 (256)
.endm

.macro restore_context
    // 恢复 EL 寄存器（ELR/SPSR）到寄存器 x17 再写回系统寄存器
    ldr     x17, [sp, #256]
    msr     elr_el1, x17

    ldr     x17, [sp, #248]
    msr     spsr_el1, x17

    // 恢复 x30（LR）
    ldr     x30, [sp, #240]

    // 恢复 x28..x0 （按与保存相反顺序也可以，但用 LDP 从低地址恢复）
    ldp     x28, x29, [sp, #224]
    ldp     x26, x27, [sp, #208]
    ldp     x24, x25, [sp, #192]
    ldp     x22, x23, [sp, #176]
    ldp     x20, x21, [sp, #160]
    ldp     x18, x19, [sp, #144]
    ldp     x16, x17, [sp, #128]     // x17 此处被覆盖（已无用）
    ldp     x14, x15, [sp, #112]
    ldp     x12, x13, [sp, #96]
    ldp     x10, x11, [sp, #80]
    ldp     x8,  x9,  [sp, #64]
    ldp     x6,  x7,  [sp, #48]
    ldp     x4,  x5,  [sp, #32]
    ldp     x2,  x3,  [sp, #16]
    ldp     x0,  x1,  [sp, #0]

    // 恢复栈指针
    add     sp, sp, #FRAME_SIZE
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
