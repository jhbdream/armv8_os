#include <printk.h>
#include <board_init.h>
#include <config.h>
#include <driver/gic.h>
#include <arch_timer.h>
#include <common/interrupt.h>
#include <kernel/task.h>
#include <kernel/tick.h>
#include <ee_stddef.h>

/**
 * @brief 定时器中断处理函数，在这里进行任务调度处理
 *
 * @param desc
 */
void timer_handler(struct irq_desc *desc)
{
    static uint64_t period;
    period = arch_timer_frequecy() / TICK_PER_SECOND;
    arch_timer_compare(period);

    //每次进入到中断时 系统定时tick增加
    g_systic = g_systic + 1;
    schedle_interrupt();
}

/**
 * @brief 初始化定时器
 *  申请中断处理函数
 *
 */
void systic_timer_init(void)
{
    if(request_irq(30, timer_handler, 0, "arch_timer", NULL) < 0)
    {
        return;
    }

    arch_timer_interrupt_disable();
    arch_timer_start();
    arch_timer_compare(arch_timer_frequecy() / TICK_PER_SECOND);
    arch_timer_interrupt_enable();
}
