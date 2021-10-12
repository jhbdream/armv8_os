#ifndef __TASK_H__

struct task
{
    void *sp;               // sp
    unsigned long spsr;     //程序状态寄存器
    unsigned long elr;      //异常返回寄存器

#if 0
    unsigned long regs[30]; //通用寄存器 x0-x29
    void *pc;               // x30
#endif
};

typedef struct task * task_t;


int task_init(struct task *t, void *sp_addr, void *pc_addr);
int task_switch_to(struct task *task_to);
int task_switch_from_to(struct task *task_from, struct task *task_to);

#endif // !__TASK_H__