#include <kernel/task.h>
#include <stdio.h>
#include <elog.h>

/**
 * @brief 全局变量，任务数组
 *
 */
struct task g_task[G_TASK_NUMBER];

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

/**
 * @brief 请求获取一个空闲任务结构体
 *
 * @return struct task* 申请到的任务
 */
struct task *requset_task()
{
    for(int i = 0; i < G_TASK_NUMBER; i++)
    {
        if(g_task[i].pid > 0)
        {
            //只要保证各个任务的pid非负并且不重复就可以
            g_task[i].pid = i;
            return &g_task[i];
        }
    }

    return NULL;
}

/**
 * @brief 将指定的任务释放掉，变为空闲状态
 *
 * @param free
 * @return long
 */
long free_task(struct task *free)
{
    long pid = free->pid;
    free->pid = -1;

    return pid;
}

/**
 * @brief 从全局任务信息中创建一个任务，返回任务的pid
 *
 * @param sp_addr
 * @param pc_addr
 * @return long
 */
long task_create(void *sp_addr, void *pc_addr)
{
    struct task *new_task;

    new_task = requset_task();
    if(new_task != NULL)
    {
        task_init(new_task, sp_addr, pc_addr);
        return new_task->pid;
    }
    else
    {
        log_e("error: can not get task!\n");
        return -1;
    }
}

/**
 * @brief 初始化全部任务为无效任务
 *
 * @return int 支持的任务数量
 */
int global_task_config(void)
{
    int i = 0;

    for(i; i < G_TASK_NUMBER; i++)
    {
        free_task(&g_task[i]);
    }

    return i;
}