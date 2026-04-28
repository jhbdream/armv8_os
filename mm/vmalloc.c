#include <stddef.h>
#include <stdint.h>
#include <asm/memory.h>
#include <mm/page.h>
#include <mm/page_alloc.h>
#include <mm/page_table.h>
#include <mm/vmalloc.h>

struct vm_struct {
    struct list_head list;
    void *addr;
    unsigned long size;
    unsigned long nr_pages;
    struct page *pages[];
};

static LIST_HEAD(vm_list);
static unsigned long vmalloc_top = VMALLOC_START;

extern pgd_t pgd_table[];

void *vmalloc(unsigned long size)
{
    struct vm_struct *vm;
    unsigned long va;
    unsigned long i;

    if (size == 0)
        return NULL;

    unsigned long nr_pages = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;

    struct page *meta_page = alloc_pages(0);
    if (!meta_page)
        return NULL;

    vm = (struct vm_struct *)page_to_virt(meta_page);
    INIT_LIST_HEAD(&vm->list);
    vm->size = size;
    vm->nr_pages = nr_pages;

    va = vmalloc_top;
    vmalloc_top += nr_pages << PAGE_SHIFT;
    vmalloc_top = (vmalloc_top + PAGE_SIZE - 1) & PAGE_MASK;

    if (vmalloc_top > VMALLOC_END) {
        free_pages(meta_page, 0);
        return NULL;
    }

    vm->addr = (void *)va;

    for (i = 0; i < nr_pages; i++) {
        struct page *p = alloc_pages(0);
        if (!p)
            goto fail;

        vm->pages[i] = p;
        map_page(pgd_table, va + i * PAGE_SIZE, page_to_phys(p), PAGE_KERNEL);
    }

    list_add(&vm->list, &vm_list);
    return vm->addr;

fail:
    while (i > 0) {
        i--;
        unmap_page(pgd_table, va + i * PAGE_SIZE);
        free_pages(vm->pages[i], 0);
    }
    vmalloc_top = va;
    free_pages(meta_page, 0);
    return NULL;
}

void vfree(void *addr)
{
    struct vm_struct *vm;
    struct list_head *pos;

    list_for_each(pos, &vm_list) {
        vm = list_entry(pos, struct vm_struct, list);
        if (vm->addr == addr) {
            for (unsigned long i = 0; i < vm->nr_pages; i++) {
                unmap_page(pgd_table, (unsigned long)addr + i * PAGE_SIZE);
                free_pages(vm->pages[i], 0);
            }

            list_del(&vm->list);

            struct page *meta_page = virt_to_page(vm);
            free_pages(meta_page, 0);
            return;
        }
    }
}
