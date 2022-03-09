#include <board_init.h>
#include <config.h>
#include <driver/gic.h>
#include <arch_timer.h>
#include <common/interrupt.h>

/**
 * @brief 关闭全局中断
 *        初始化中断控制器(GIC V3)
 *
 * @return int
 */
int interrupt_init(void)
{
    local_irq_disable();
    gic_init();
}
