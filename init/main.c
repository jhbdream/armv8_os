#include <stddef.h>

#include <ee/init.h>
#include <ee/irqflags.h>

#include <asm/memory.h>

#include <mm/memblock.h>
#include <mm/page_alloc.h>

#include <driver/console.h>

#include <printk.h>

extern void buddy_page_test(void);
extern void slob_test(void);

int gicv3_driver_init(void);

extern unsigned long __kimage_start[], __kimage_end[];

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
    u64 base, size;

    base = 0x40000000;
    size = 0x40000000;

    local_irq_disable();

    setup_arch();

    console_init();

    eeos_printlogo();

    /* 初始化memblock内存管理器 */
    memblock_debug_set(0);

    memblock_add(base, size);
    memblock_reserve(virt_to_phys(__kimage_start), __kimage_end - __kimage_start);

    /* 从 memblock 分配page数据结构 */
    vmemmap_page_init(base, base + size);

    memblock_dump_all();

    buddy_zone_init();

    // 把 memblock 剩余可以使用内存分配到 page 管理器
    free_memory_core();

    gicv3_driver_init();

    // slob_test();

#if 0

    buddy_page_test();

	u64 base = memblock_start_of_DRAM();
	u64 size = memblock_end_of_DRAM() - memblock_start_of_DRAM();

#    if 0
   extern void buddy_page_test(void);
   buddy_page_test();

   extern void slob_test(void);
   slob_test();
#    endif

#    ifdef CONFIG_FLAT_TEST
	extern void fdt_test(void);
	fdt_test();
#    endif

	printk("hello kernel!\n");

	int vmalloc_test(void);
	vmalloc_test();
#endif

    for (;;)
        ;
}
