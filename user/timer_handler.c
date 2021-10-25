#include <stdio.h>
#include <board_init.h>
#include <elog.h>
#include <config.h>
#include <driver/gic.h>
#include <arch_timer.h>
#include <common/interrupt.h>

int count = 0;

extern struct task taska;
extern struct task taskb;
extern void interrupt_task_switch_from_to(struct task *task_from, struct task *task_to);

void timer_handler(struct irq_desc *desc)
{
    arch_timer_compare(arch_timer_frequecy());
    log_i("arch timer_handler!");

    if((count++) % 2 == 0)
    {
        log_i("switch to taskb!");
        interrupt_task_switch_from_to(&taska, &taskb);
    }
    else
    {
        log_i("switch to taska!");
        interrupt_task_switch_from_to(&taskb, &taska);
    }
}

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