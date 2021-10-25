#ifndef __TASK_H__

struct task
{
    void *sp;               // sp
    unsigned long spsr;     //程序状态寄存器
    unsigned long elr;      //异常返回寄存器

    long pid;               //任务id,非负数时表示有效任务
};

typedef struct task * task_t;


int task_init(struct task *t, void *sp_addr, void *pc_addr);
int task_switch_to(struct task *task_to);
int task_switch_from_to(struct task *task_from, struct task *task_to);
void interrupt_task_switch_from_to(struct task *task_from, struct task *task_to);


#define G_TASK_NUMBER 64
extern struct task g_task[G_TASK_NUMBER];

#endif // !__TASK_H__