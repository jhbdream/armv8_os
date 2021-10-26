#include <kernel/task.h>
#include <stdio.h>
#include <elog.h>

/**
 * @brief 全局变量，任务数组
 *
 */
struct task g_task[G_TASK_NUMBER];

/**
 * @brief 保存与记录当前正在执行的任务
 *
 */
struct task *g_current_task = NULL;

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

    return 0;
}

/**
 * @brief 请求获取一个空闲任务结构体
 *
 * @return struct task* 申请到的任务
 */
static struct task *requset_task()
{
    for(int i = 0; i < G_TASK_NUMBER; i++)
    {
        if(g_task[i].pid >= 0)
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
static long free_task(struct task *free)
{
    long pid = free->pid;
    free->pid = -1;

    return pid;
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
        log_e("error: can not get task!");
        return -1;
    }
}

struct task *task_schedule_alog()
{
    // 获取下一个任务
    struct task *next_task;
    if(g_current_task == &g_task[G_TASK_NUMBER - 1])
    {
        next_task = &g_task[0];
    }
    else
    {
        next_task = g_current_task + 1;
    }

    for(int i = 0; i < G_TASK_NUMBER; i++)
    {
        if(next_task->pid >= 0)
        {
            //找到一个有效任务 返回任务
            return next_task;
        }
        else if (next_task == &g_task[G_TASK_NUMBER - 1])
        {
            //如果遍历到了末尾，回到头部
            next_task = &g_task[0];
        }
    }

    //目前没有就绪任务，返回空 后续可以考虑添加空闲idel任务
    return NULL;
}