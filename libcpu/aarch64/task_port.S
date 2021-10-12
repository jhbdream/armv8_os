    .global task_switch_to
    .type task_switch_to, "function"

//根据C语言和汇编语言的调用过程，第一个参数存放在x0寄存器中
//我们函数传递的是任务结构体指针
task_switch_to:
    // 恢复状态寄存器
    mov x1, x0
    add x1, x1, #8 * 1
    ldr x2, [x1]
    msr spsr_el1, x2

    // 恢复异常返回地址
    mov x1, x0
    add x1, x1, #8 * 2
    ldr x2, [x1]
    msr elr_el1, x2

    //恢复通用寄存器现场
    ldr x1, [x0]
    mov sp, x1

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

    //异常返回
    //1 cpsr = spsr
    //2 pc = elr_el1
    eret

    .global task_switch_from_to
    .type task_switch_from_to, "function"
// x0: from
// x1: to
task_switch_from_to:
    //保存当前现场到from a主动调用 b中断切换
