#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <eeos/irqflags.h>

#include <kernel/task.h>

/**
 * @brief
 *
 *  TASK_STATE_NONE ----->> TASK_STATE_READY <<----->> TASK_STATE_SLEEP
 */

#define TASK_STATE_NONE     BIT(0) // 初始任务状态
#define TASK_STATE_READY    BIT(1) // 任务已经创建，未运行
#define TASK_STATE_RUN      BIT(2) // 任务已经创建，并且正在运行(保留)
#define TASK_STATE_SLEEP    BIT(3) // 任务睡眠
#define TASK_STATE_WAIT_SEM BIT(4) // 等待信号量挂起

#define TASK_NUMBER         64

/**
 * @brief 全局变量，任务数组
 *
 */
struct task g_task[TASK_NUMBER];

/**
 * @brief 保存与记录当前正在执行的任务
 *
 */
struct task *g_current_task = NULL;

/**
 * @brief 从全局的任务信息数组中申请分配一个任务
 *
 * @return struct task* 申请到的任务
 */
static struct task *requset_task(void)
{
    int i;

    for (i = 0; i < TASK_NUMBER; i++) {

        // 找到一个空闲任务块
        if (g_task[i].pid == -1) {
            // 只要保证各个任务的pid非负并且不重复就可以
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
 * @return uint32_t 返回被释放任务的pid
 */
static void free_task(struct task *free)
{
    free->pid        = -1;
    free->priority   = -1;
    free->task_state = TASK_STATE_NONE;
}

/**
 * @brief 初始化全部任务为无效任务
 *
 * @return int 支持的任务数量
 */
static int task_deinit(void)
{
    int i;

    for (i = 0; i < TASK_NUMBER; i++) {
        free_task(&g_task[i]);
    }

    return i;
}

/**
 * @brief 对外的内核任务初始化接口
 *
 */
void kernel_task_init(void)
{
    task_deinit();
}

/**
 * @brief 初始化一个静态的任务
 *
 * @param t 任务结构体变量指针
 * @param sp_addr 该任务的堆栈地址
 * @param pc_addr 该任务的函数地址
 * @return int
 */
static void task_init(struct task *t, char *name, void *sp_addr, void *pc_addr,
                      long priority)
{
    int       i;
    uint64_t *sp_init = sp_addr;

    // 初始化任务栈
    for (i = 0; i < 31; i++) {
        sp_init  = sp_init - 1;
        *sp_init = 30 - i;
    }

    t->sp   = sp_init;
    t->elr  = (unsigned long)pc_addr; // 任务的入口地址
    t->spsr = 0x345; // 在aarch64架构下 设置切换到el1 并且使能全局中断

    t->priority   = priority;
    t->task_state = TASK_STATE_READY;

    strcpy(t->task_name, name);
}

/**
 * @brief 动态获取一个任务，并且完成任务的初始化
 *
 * @param sp_addr
 * @param pc_addr
 * @return long
 */
struct task *task_create(char *name, void *sp_addr, void *pc_addr, long priority)
{
    struct task *new_task;

    new_task = requset_task();

    if (new_task != NULL) {
        task_init(new_task, name, sp_addr, pc_addr, priority);
        return new_task;
    } else {
        return NULL;
    }
}

/**
 * @brief 平均模式任务调度器
 *        遍历任务列表，查找下一个有效任务
 *        如果找不到，返回NULL
 *
 * @return struct task*
 */
struct task *task_schedule_alog_average(void)
{
    int i;

    // 获取下一个任务
    struct task *next_task;

    if (g_current_task == &g_task[TASK_NUMBER - 1]) {
        // 如果现在任务是在任务快末尾，从任务快开头遍历
        next_task = &g_task[0];
    } else {
        next_task = g_current_task + 1;
    }

    for (i = 0; i < TASK_NUMBER; i++) {
        if (next_task->task_state & TASK_STATE_READY) {
            // 找到一个有效任务 返回任务
            return next_task;
        } else if (next_task == &g_task[TASK_NUMBER - 1]) {
            // 如果遍历到了末尾，回到头部
            next_task = &g_task[0];
        } else {
            // 遍历下一个任务块
            next_task = next_task + 1;
        }
    }

    // 目前没有就绪任务，返回空 后续可以考虑添加空闲idel任务
    return NULL;
}

/**
 * @brief 在中断环境下进行任务切换
 *
 */
void schedle_interrupt(void)
{
    static struct task *from;
    static struct task *to;

    from = g_current_task;
    to   = task_schedule_alog_average();

    // TODO: 没有需要调度的任务？
    if (from == NULL || to == NULL) {
        return;
    }

    // 同一个任务 也不需要触发切换
    if (from == to) {
        return;
    }

    interrupt_task_switch_from_to(from, to);
}

/**
 * @brief 在非中断环境下进行任务切换
 *
 */
void schedle(void)
{
    unsigned long flag;

    static struct task *from;
    static struct task *to;

    local_irq_save(flag);

    from = g_current_task;
    to   = task_schedule_alog_average();

    // TODO: 没有需要调度的任务？
    if (from == NULL || to == NULL) {
        local_irq_restore(flag);
        return;
    }

    // 同一个任务
    if (from == to) {
        local_irq_restore(flag);
        return;
    }

    task_switch_from_to(from, to);

    local_irq_restore(flag);
}

/**
 * @brief 获取当前任务的pid
 *
 * @return uint32_t
 */
uint32_t getpid(void)
{
    return g_current_task->pid;
}
