#include <stddef.h>

#include <eeos/irqflags.h>

#include <asm/memory.h>

#include <driver/console.h>

#include <printk.h>

#include <mm/page_alloc.h>
#include <mm/vmalloc.h>

void arm64_arch_timer_init(void);
void setup_arch(void);

int gicv3_driver_init(void);

static void eeos_printlogo(void)
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

static void mm_test(void)
{
    struct page *p;
    void *v;
    char *buf;

    /* Buddy allocator test */
    p = alloc_pages(0);
    if (p) {
        buf = (char *)page_to_virt(p);
        buf[0] = 'A';
        buf[1] = 'B';
        buf[2] = 'C';
        buf[3] = '\0';
        printk("buddy test: alloc_pages(0) ok, data=%s\n", buf);
        free_pages(p, 0);
        printk("buddy test: free_pages ok\n");
    } else {
        printk("buddy test: alloc_pages failed\n");
    }

    /* Multi-page buddy test */
    p = alloc_pages(2);
    if (p) {
        printk("buddy test: alloc_pages(2) ok (16KB)\n");
        free_pages(p, 2);
    } else {
        printk("buddy test: alloc_pages(2) failed\n");
    }

    /* vmalloc test */
    v = vmalloc(8192);
    if (v) {
        buf = (char *)v;
        for (int i = 0; i < 16; i++)
            buf[i] = 'a' + i;
        buf[15] = '\0';
        printk("vmalloc test: vmalloc(8KB) ok, data=%s\n", buf);
        vfree(v);
        printk("vmalloc test: vfree ok\n");
    } else {
        printk("vmalloc test: vmalloc failed\n");
    }
}

void start_kernel(void)
{
    local_irq_disable();

    setup_arch();

    console_init();

    gicv3_driver_init();

    arm64_arch_timer_init();

    eeos_printlogo();

    mm_init();
    mm_test();

    printk("\nAll mm tests passed.\n");

    for (;;)
        ;
}
