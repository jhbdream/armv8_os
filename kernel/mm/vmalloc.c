#include "asm/page-def.h"
#include "mm/memblock.h"
#include "mm/slab.h"
#include "printk.h"
#include <type.h>
#include <eeos.h>
#include <stddef.h>
#include <asm-generic/bug.h>
#include <ee/align.h>

struct vm_struct {
	void *addr;
	unsigned long size;
	unsigned long flags;
	struct page **page;
	unsigned int nr_pages;
	unsigned long phys_addr;
	struct vm_struct *next;
};

struct vm_struct *vmlist = NULL;

/**
 * @brife 从地址范围区间内分配一片内存
 *
 * @Param size 分配内存空间大小
 * @Param flags
 * @Param start 分配范围起始地址
 * @Param end 分配范围结束地址
 *
 * @return
 */
struct vm_struct *__get_vm_area(unsigned long size, unsigned long flags,
		unsigned long start, unsigned long end)
{
	struct vm_struct **p, *tmp, *area;
	unsigned long align = 1;
	unsigned long addr;

	/**
	 * 分配大小按照 page_size 对齐
	 */
	addr = ALIGN(start, align);
	size = ALIGN(size, PAGE_SIZE);
	if(!size)
		return NULL;

	/**
	 * 分配一个链表节点
	 */
	area = kmalloc(sizeof(*area));
	if(!area)
		return NULL;

	/**
	 *
	 * lock: vmlist 是全局变量,如果对全局变量有写相关修改 应该进行加锁操作
	 * 遍历记录的 vmstruct 信息，找到空闲的区域
	 */
	for(p = &vmlist; (tmp = *p) != NULL; p = &tmp->next)
	{
		/* 更新起始地址 */
		if((unsigned long)tmp->addr < addr)
		{
			if((unsigned long)tmp->addr + tmp->size >= addr)
				addr = ALIGN(tmp->size + (unsigned long)tmp->addr, align);

			continue;
		}

		/* addr 会变化，避免溢出 */
		if(size + addr < addr)
			goto out;

		/**
		 * 找到一片足够大小的空闲区域
		 *
		 * 找到并退出有俩种条件：
		 * 1. 链表找到末尾(要判断到末尾时剩余空间是否足够)
		 * 2. 在中间找到空隙
		 */
		if(size + addr <= (unsigned long)tmp->addr)
			goto found;

		/* 更新查找起始地址 并判断剩余空间是否足够 */
		addr = ALIGN(tmp->size + (unsigned long)tmp->addr, align);
		if(addr > end - size)
			goto out;
	}

found:
	/**
	 *  创建并初始化一个 vmstruct 节点
	 *  插入到链表中
	 *
	 */
	area->next = *p;
	*p = area;

	area->flags = flags;
	area->addr = (void *)addr;
	area->size = size;
	area->page = NULL;
	area->nr_pages = 0;
	area->phys_addr = 0;

	/* unlock */
	return area;
out:
	kfree(area);
	/* unlock */
	return NULL;
}

int vmalloc_test(void)
{
	struct vm_struct *vm;

	printk("do vmalloc_test!\n");
	for(int i = 1; i < 32; i++)
	{
		vm = __get_vm_area(i * 4096, 0, 0, 0x40000000);
		if(!vm)
		{
			printk("__get_vm_area failed!\n");
			return -1;
		}
		printk("index[%d]: vm addr [0x%08x] size: 0x%04x \n", i, vm->addr, i * 4096);
	}

	return 0;
}










