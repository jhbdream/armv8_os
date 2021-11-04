#ifndef __TASK_H__

struct task
{
    /* task contex */
    void *sp;               // sp
    unsigned long spsr;     //程序状态寄存器
    unsigned long elr;      //异常返回寄存器

    /* task info */
    long pid;               //任务id,非负数时表示有效任务
    long priority;          //任务优先级，数字越大表示优先级越高 优先级范围 [0 - (G_TASK_MAX_PRIORITY - 1)]
                            //定义最低的优先级为 idle 空闲任务(G_TASK_MAX_PRIORITY - 1)
};

typedef struct task *task_t;

int task_switch_to(struct task *task_to);
int task_switch_from_to(struct task *task_from, struct task *task_to);
void interrupt_task_switch_from_to(struct task *task_from, struct task *task_to);

int global_task_config(void);

struct task *task_create(void *sp_addr, void *pc_addr, long priority);
int task_init(struct task *t, void *sp_addr, void *pc_addr, long priority);

struct task *task_schedule_alog_average();
struct task *task_schedule_alog_priority();

void kernel_task_init(void);

#define G_TASK_MAX_PRIORITY 256
#define G_TASK_NUMBER 64

extern struct task g_task[G_TASK_NUMBER];
extern struct task *g_current_task;

#endif // !__TASK_H__
