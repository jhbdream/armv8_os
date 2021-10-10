    .global task_switch_to
    .type task_switch_to, "function"

//根据C语言和汇编语言的调用过程，第一个参数存放在x0寄存器中
//我们函数传递的是任务结构体指针
task_switch_to:
    ldr x1, [x0]    //任务的pc地址是第一个参数，读取到x1寄存器
    mov x30, x1     //把pc地址放入到x30寄存器，x30寄存器是lr寄存器，稍后切换完成以后返回时候就会返回到任务中执行

    add x0, x0, 8   //sp参数在结构体中偏移是八字节
    ldr x1, [x0]    //取出并设置sp地址
    mov sp, x1

    ret             //函数返回，跳转到x30寄存器存放地址去执行

    .global task_switch_from_to
    .type task_switch_from_to, "function"

task_switch_from_to: