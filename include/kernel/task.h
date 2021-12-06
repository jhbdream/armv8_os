#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>
#include <bits.h>

#define TASK_NAME_LEN 64

struct task
{
    /* task contex */
    void *sp;               //记录任务切换过程中的cpu堆栈寄存器
    unsigned long spsr;     //记录程序状态寄存器
    unsigned long elr;      //记录异常返回寄存器

    /* task info */
    uint32_t pid;               //任务id,非负数时表示有效任务
    uint32_t priority;          //任务优先级，数字越大表示优先级越高 优先级范围 [0 - (G_TASK_MAX_PRIORITY - 1)]
                            //定义最低的优先级为 idle 空闲任务(G_TASK_MAX_PRIORITY - 1)
    uint32_t task_state;
    uint64_t sleep_timeout;
    char task_name[TASK_NAME_LEN];
};

/**
 * @brief
 *
 *  TASK_STATE_NONE ----->> TASK_STATE_READY <<----->> TASK_STATE_SLEEP
 */

#define TASK_STATE_NONE     BIT(0)  //初始任务状态
#define TASK_STATE_READY    BIT(1)  //任务已经创建，未运行
#define TASK_STATE_RUN      BIT(2)  //任务已经创建，并且正在运行(保留)
#define TASK_STATE_SLEEP    BIT(3)  //任务睡眠
#define TASK_STATE_WAIT_SEM    BIT(4)  //等待信号量挂起


typedef struct task *task_t;

#define G_TASK_MAX_PRIORITY 256
#define G_TASK_NUMBER 64

extern struct task g_task[G_TASK_NUMBER];
extern struct task *g_current_task;

int task_switch_to(struct task *task_to);
int task_switch_from_to(struct task *task_from, struct task *task_to);
void interrupt_task_switch_from_to(struct task *task_from, struct task *task_to);

struct task *task_create(char *name, void *sp_addr, void *pc_addr, long priority);
struct task *task_schedule_alog_average(void);
struct task *task_schedule_alog_priority(void);

void schedle_interrupt(void);
void schedle(void);

void kernel_task_init(void);
uint32_t getpid(void);

#endif // !__TASK_H__
