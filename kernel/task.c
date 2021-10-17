#include <kernel/task.h>

/**
 * @brief 创建一个任务
 *
 * @param t 任务结构体变量指针
 * @param sp_addr 该任务的堆栈地址
 * @param pc_addr 该任务的起始地址
 * @return int
 */
int task_init(struct task *t, void *sp_addr, void *pc_addr)
{

    t->sp = sp_addr;
    t->elr = (unsigned long)pc_addr;    //eret run addr
    t->spsr = 0x345;    //ebable irq run in el1

#if 0
    t->regs[0] = 0x1234abcd;
    t->regs[1] = 0x1234abcd;
    t->regs[2] = 0x1234abcd;
    t->regs[3] = 0x1234abcd;
    t->regs[4] = 0x1234abcd;
    t->regs[5] = 0x1234abcd;
    t->regs[6] = 0x1234abcd;
    t->regs[7] = 0x1234abcd;
    t->regs[8] = 0x1234abcd;
    t->regs[9] = 0x1234abcd;
    t->regs[10] = 0x1234abcd;
    t->regs[11] = 0x1234abcd;
    t->regs[12] = 0x1234abcd;
    t->regs[13] = 0x1234abcd;
    t->regs[14] = 0x1234abcd;
    t->regs[15] = 0x1234abcd;
    t->regs[16] = 0x1234abcd;
    t->regs[17] = 0x1234abcd;
    t->regs[18] = 0x1234abcd;
    t->regs[19] = 0x1234abcd;
    t->regs[20] = 0x1234abcd;
    t->regs[21] = 0x1234abcd;
    t->regs[22] = 0x1234abcd;
    t->regs[23] = 0x1234abcd;
    t->regs[24] = 0x1234abcd;
    t->regs[25] = 0x1234abcd;
    t->regs[26] = 0x1234abcd;
    t->regs[27] = 0x1234abcd;
    t->regs[28] = 0x1234abcd;
    t->regs[29] = 0x1234abcd;
#endif

    return 0;
}
