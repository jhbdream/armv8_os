
void start_kernel(void)
{
#if 0
	local_irq_disable();

	setup_arch();

	console_init();

	printk("VERSION: %s\n", EEOS_VERSION_STR);

	memblock_dump_all();

	u64 base = memblock_start_of_DRAM();
	u64 size = memblock_end_of_DRAM() - memblock_start_of_DRAM();
	vmemmap_page_init(base, base + size);

	buddy_zone_init();
	free_memory_core();

#if 0
   extern void buddy_page_test(void);
   buddy_page_test();

   extern void slob_test(void);
   slob_test();
#endif

#ifdef CONFIG_FLAT_TEST
	extern void fdt_test(void);
	fdt_test();
#endif

	printk("hello kernel!\n");

	int vmalloc_test(void);
	vmalloc_test();

#endif

	for (;;)
		;
}
