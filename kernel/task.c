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
    t->pc = pc_addr;

    return 0;
}

#if 0
int task_switch_to(struct task *task_to)
{

}

int task_switch_from_to(struct task *task_from, struct task *task_to)
{

}
#endif