#include <stdint.h>
#include <stddef.h>
#include <kernel/printk.h>
#include <asm/memory.h>
#include <mm/page.h>
#include <mm/page_alloc.h>

#define PG_free (1UL << 0)

#define PFN_DOWN(x)  (((unsigned long)(x)) >> PAGE_SHIFT)
#define PFN_UP(x)    ((((unsigned long)(x)) + PAGE_SIZE - 1) >> PAGE_SHIFT)

static struct free_area {
    struct list_head free_list;
    unsigned long nr_free;
} free_area[MAX_ORDER + 1];

static struct page *page_array;
static unsigned long total_pages;
static unsigned long free_pages_count;

static inline unsigned long page_to_pfn(struct page *page)
{
    return page - page_array;
}

static inline struct page *pfn_to_page(unsigned long pfn)
{
    return page_array + pfn;
}

unsigned long page_to_phys(struct page *page)
{
    return PHYS_OFFSET + (page_to_pfn(page) << PAGE_SHIFT);
}

struct page *phys_to_page(unsigned long phys)
{
    return pfn_to_page((phys - PHYS_OFFSET) >> PAGE_SHIFT);
}

void *page_to_virt(struct page *page)
{
    return phys_to_virt(page_to_phys(page));
}

struct page *virt_to_page(void *vaddr)
{
    return phys_to_page(virt_to_phys(vaddr));
}

struct page *alloc_pages(unsigned int order)
{
    struct page *page;
    unsigned int current_order;

    if (order > MAX_ORDER)
        return NULL;

    current_order = order;
    while (current_order <= MAX_ORDER && list_empty(&free_area[current_order].free_list))
        current_order++;

    if (current_order > MAX_ORDER)
        return NULL;

    page = list_first_entry(&free_area[current_order].free_list, struct page, lru);
    list_del(&page->lru);
    free_area[current_order].nr_free--;

    while (current_order > order) {
        current_order--;
        unsigned long buddy_pfn = page_to_pfn(page) + (1UL << current_order);
        struct page *buddy = pfn_to_page(buddy_pfn);
        buddy->flags |= PG_free;
        buddy->order = current_order;
        list_add(&buddy->lru, &free_area[current_order].free_list);
        free_area[current_order].nr_free++;
    }

    page->flags &= ~PG_free;
    page->order = order;
    page->refcount = 1;
    free_pages_count -= (1UL << order);

    return page;
}

void free_pages(struct page *page, unsigned int order)
{
    unsigned long pfn = page_to_pfn(page);
    struct page *merged_page;

    if (order > MAX_ORDER)
        return;

    while (order < MAX_ORDER) {
        unsigned long buddy_pfn = pfn ^ (1UL << order);

        if (buddy_pfn >= total_pages)
            break;

        struct page *buddy = pfn_to_page(buddy_pfn);

        if (!(buddy->flags & PG_free) || buddy->order != order)
            break;

        list_del(&buddy->lru);
        free_area[order].nr_free--;
        buddy->flags &= ~PG_free;

        pfn &= buddy_pfn;
        order++;
    }

    merged_page = pfn_to_page(pfn);
    merged_page->flags |= PG_free;
    merged_page->order = order;
    list_add(&merged_page->lru, &free_area[order].free_list);
    free_area[order].nr_free++;
    free_pages_count += (1UL << order);
}

void mm_init(void)
{
    extern unsigned long __kimage_end[];

    total_pages = PHYS_SIZE >> PAGE_SHIFT;

    for (int i = 0; i <= MAX_ORDER; i++) {
        INIT_LIST_HEAD(&free_area[i].free_list);
        free_area[i].nr_free = 0;
    }

    unsigned long kernel_phys_end = (unsigned long)__kimage_end - kimage_voffset;
    kernel_phys_end = (kernel_phys_end + PAGE_SIZE - 1) & PAGE_MASK;

    unsigned long page_array_size = total_pages * sizeof(struct page);
    page_array_size = (page_array_size + PAGE_SIZE - 1) & PAGE_MASK;
    unsigned long page_array_phys = kernel_phys_end;

    page_array = (struct page *)phys_to_virt(page_array_phys);

    for (unsigned long i = 0; i < total_pages; i++) {
        INIT_LIST_HEAD(&page_array[i].lru);
        page_array[i].flags = 0;
        page_array[i].order = 0;
        page_array[i].refcount = 0;
    }

    unsigned long reserved_end = page_array_phys + page_array_size;
    reserved_end = (reserved_end + PAGE_SIZE - 1) & PAGE_MASK;
    unsigned long free_start_pfn = (reserved_end - PHYS_OFFSET) >> PAGE_SHIFT;

    free_pages_count = 0;
    unsigned long pfn = free_start_pfn;
    while (pfn < total_pages) {
        unsigned int order = MAX_ORDER;
        while (order > 0) {
            unsigned long block_size = 1UL << order;
            if ((pfn & (block_size - 1)) == 0 && pfn + block_size <= total_pages)
                break;
            order--;
        }

        struct page *page = pfn_to_page(pfn);
        page->flags |= PG_free;
        page->order = order;
        list_add_tail(&page->lru, &free_area[order].free_list);
        free_area[order].nr_free++;
        free_pages_count += (1UL << order);
        pfn += (1UL << order);
    }

    printk("mm: %lu pages total, %lu pages free (%lu KB)\n",
           total_pages, free_pages_count, (free_pages_count * PAGE_SIZE) / 1024);
}
