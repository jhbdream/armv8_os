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
        //找到一个空闲任务块
        if(g_task[i].pid == -1)
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
 * @return long 返回被释放任务的pid
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
 * @brief 从全局任务信息中创建一个任务，返回任务块信息
 *
 * @param sp_addr
 * @param pc_addr
 * @return long
 */
struct task *task_create(void *sp_addr, void *pc_addr)
{
    struct task *new_task;

    new_task = requset_task();
    if(new_task != NULL)
    {
        task_init(new_task, sp_addr, pc_addr);
        return new_task;
    }
    else
    {
        log_e("error: can not get task!");
        return NULL;
    }
}

struct task *task_schedule_alog()
{
    // 获取下一个任务
    struct task *next_task;

    if(g_current_task == &g_task[G_TASK_NUMBER - 1])
    {
        //如果现在任务是在任务快末尾，从任务快开头遍历
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
        else
        {
            //遍历下一个任务块
            next_task = next_task + 1;
        }
    }

    //目前没有就绪任务，返回空 后续可以考虑添加空闲idel任务
    return NULL;
}