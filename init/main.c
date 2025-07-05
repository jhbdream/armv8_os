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
    local_irq_disable();

    setup_arch();

    console_init();

    eeos_printlogo();

    gicv3_driver_init();

    arm64_arch_timer_init();

    for (;;)
        ;
}
