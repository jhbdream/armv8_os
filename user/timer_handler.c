#include <stdio.h>
#include <board_init.h>
#include <elog.h>
#include <config.h>
#include <driver/gic.h>
#include <arch_timer.h>
#include <common/interrupt.h>
#include <kernel/task.h>

/**
 * @brief 定时器中断处理函数，在这里进行任务调度处理
 *
 * @param desc
 */
void timer_handler(struct irq_desc *desc)
{
    static struct task *from;
    static struct task *to;

    arch_timer_compare(arch_timer_frequecy());

    from = g_current_task;
    to = task_schedule_alog_priority();

    log_i("switch from pid:[%d]!", from->pid);
    log_i("switch to pid:[%d]!", to->pid);

    interrupt_task_switch_from_to(from, to);
}

/**
 * @brief 初始化定时器
 *  申请中断处理函数
 *
 */
void timer_init(void)
{
    if(request_irq(30, timer_handler, 0, "arch_timer", NULL) < 0)
    {
        log_e("request_irq failed!");
        return;
    }

    arch_timer_interrupt_disable();
    arch_timer_start();
    arch_timer_compare(arch_timer_frequecy());
    arch_timer_interrupt_enable();
}