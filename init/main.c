#include <stddef.h>

#include <eeos/irqflags.h>

#include <asm/memory.h>

#include <mm/memblock.h>
#include <mm/page_alloc.h>

#include <driver/console.h>

#include <kernel/task.h>

#include <printk.h>

extern unsigned long __kimage_start[], __kimage_end[];

void arm64_arch_timer_init(void);
void idle_task_init(void);
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
    u64 phys_base, phys_size;

    phys_base = 0x40000000;
    phys_size = 0x40000000;

    local_irq_disable();

    setup_arch();

    console_init();

    eeos_printlogo();

    // 1. 添加全部物理内存
    memblock_add(phys_base, phys_size);

    // 2. 保留内核镜像物理内存
    memblock_reserve(virt_to_phys(__kimage_start), __kimage_end - __kimage_start);

    // 3. 从 memblock 分配page数据结构
    vmemmap_page_init(phys_base, phys_base + phys_size);

    buddy_zone_init();

    // 4. 把 memblock 剩余可以使用内存分配到 page 管理器
    free_memory_core();

    // 5. 中断初始化
    gicv3_driver_init();

    // 6. 定时器初始化 用于任务切换调度
    arm64_arch_timer_init();

    // 7. 内核任务初始化
    kernel_task_init();

    idle_task_init();

    for (;;)
        ;
}
