#ifndef __TASK_H__

struct task
{
    void *pc;
    void *sp;
    unsigned long regs[32];
};

typedef struct task * task_t;


int task_init(struct task *t, void *sp_addr, void *pc_addr);
int task_switch_to(struct task *task_to);
int task_switch_from_to(struct task *task_from, struct task *task_to);

#endif // !__TASK_H__