#include <stddef.h>

#include <eeos/irqflags.h>

#include <asm/memory.h>

#include <driver/console.h>

#include <printk.h>

void arm64_arch_timer_init(void);
void setup_arch(void);

int gicv3_driver_init(void);

void eeos_printlogo(void)
{
    printk("\n"
           "    #######  #######   #####    #######\n"
           "    ##       ##       ##   ##   #      \n"
           "    ######   #######  ##   ##   ######\n"
           "    ##       ##       ##   ##        #\n"
           "    #######  #######   #####    #######\n"
           "\n"
           "    Embedded Easy Operating System\n"
           "\n");
}

void start_kernel(void)
{
    // 关闭全局中断
    local_irq_disable();

    // 初始化内存映射
    setup_arch();

    // 初始化串口控制台
    console_init();

    // 初始化中断控制器
    gicv3_driver_init();

    // 初始化定时器
    arm64_arch_timer_init();

    // 输出 LOGO
    eeos_printlogo();

    local_irq_enable();

    for (;;)
        ;
}
