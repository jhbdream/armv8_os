#ifndef __TASK_H__
#include <stdint.h>
#include <bits.h>

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
    uint32_t task_flag;
    uint64_t sleep_timeout;
};

#define TASK_FLAG_NONE      BIT(0)
#define TASK_FLAG_RUN       BIT(1)
#define TASK_FLAG_SLEEP     BIT(2)

typedef struct task *task_t;

#define G_TASK_MAX_PRIORITY 256
#define G_TASK_NUMBER 64

extern struct task g_task[G_TASK_NUMBER];
extern struct task *g_current_task;

int task_switch_to(struct task *task_to);
int task_switch_from_to(struct task *task_from, struct task *task_to);
void interrupt_task_switch_from_to(struct task *task_from, struct task *task_to);

int task_init(struct task *t, void *sp_addr, void *pc_addr, long priority);
struct task *task_create(void *sp_addr, void *pc_addr, long priority);

struct task *task_schedule_alog_average(void);
struct task *task_schedule_alog_priority(void);

void schedle_interrupt(void);
void schedle(void);

void kernel_task_init(void);
uint32_t getpid(void);

#endif // !__TASK_H__
