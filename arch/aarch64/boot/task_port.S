    .global g_current_task

    .global task_switch_to
    .type task_switch_to, "function"

//根据C语言和汇编语言的调用过程，第一个参数存放在x0寄存器中
//我们函数传递的是任务结构体指针
task_switch_to:

    // 即将进行任务切换，更新to任务到当前任务信息
    adr x2, g_current_task
    str x0, [x2]

    //1 恢复任务的堆栈寄存器
    mov x1, x0
    add x1, x1, #8 * 0
    ldr x2, [x1]
    mov sp, x2


    //2.恢复任务的状态寄存器
    mov x1, x0
    add x1, x1, #8 * 1
    ldr x2, [x1]
    msr spsr_el1, x2

    //3.恢任务的复异常返回地址
    mov x1, x0
    add x1, x1, #8 * 2
    ldr x2, [x1]
    msr elr_el1, x2

    //4. 恢复任务的通用寄存器
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
    sub sp, sp, #30 * 8

    //异常返回
    //1 cpsr = spsr
    //2 pc = elr_el1
    eret

    .global task_switch_from_to
    .type task_switch_from_to, "function"
// x0: from
// x1: to
task_switch_from_to:
    //由from任务切换到to任务
    //第一件事情，保存from任务的状态

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
    mov x3, x30
    mov x2, x0
    add x2, x2, #8 * 2
    str x3, [x2]


    // 即将进行任务切换，更新to任务到当前任务信息
    adr x2, g_current_task
    str x1, [x2]

    //恢复to任务的状态

    //1 恢复sp寄存器
    mov x2, x1
    add x2, x2, #8 * 0
    ldr x3, [x2]
    mov sp, x3


    //2 恢复spsr状态寄存器
    mov x2, x1
    add x2, x2, #8 * 1
    ldr x3, [x2]
    msr spsr_el1, x3

    //3 恢复异常返回地址
    mov x2, x1
    add x2, x2, #8 * 2
    ldr x3, [x2]
    msr elr_el1, x3

    //4 恢复通用寄存器
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

	//该指令原意是从异常返回
    // 1. 把spsr寄存器恢复到cpcr寄存器中
    // 2. 把elr_el1 寄存器中的数据作为下一条指令的地址，跳转执行
    // 3. 所以通过该指令可以跳转到目标任务中
    eret


.global task_interrupt_from_thread;
.global task_interrupt_to_thread;
.global task_thread_switch_interrupt_flag;

    .global interrupt_task_switch_from_to
    .type interrupt_task_switch_from_to, "function"
// x0: from
// x1: to
//设置切换标志为1，通过全局变量传递from和to任务信息
interrupt_task_switch_from_to:
    adr x2, task_thread_switch_interrupt_flag
    mov x3, #1
    str x3, [x2]

    adr x2, task_interrupt_from_thread
    mov x3, x0
    str x3, [x2]

    adr x2, task_interrupt_to_thread
    mov x3, x1
    str x3, [x2]

    ret
