#include <arch_timer.h>
#include <driver/interrupt.h>
#include <stddef.h>
#include <printk.h>

void timer_handler(struct irq_desc *desc)
{
    static uint64_t period;
    period = arch_timer_frequecy();
    arch_timer_compare(period);

    printk("timer handler!\n");
}

void aarch64_timer_init(void)
{
    if(request_irq(30, timer_handler, 0, "arch_timer", NULL) < 0)
    {
        return;
    }

    arch_timer_interrupt_disable();
    arch_timer_start();
    arch_timer_compare(arch_timer_frequecy());
    arch_timer_interrupt_enable();
}