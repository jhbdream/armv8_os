#include <ee_stdio.h>
#include <ee_stddef.h>
#include <printk.h>
#include <mm/simple_mm.h>

static mm_node_t *head_node = NULL;

#define SIZEOF_MM_NODE (((sizeof(mm_node_t)) + (8) - 1) & ~((8) - 1))

void node_dump(mm_node_t *node, const char *tag)
{
    printk("=====[%s]:node dump=====\n", tag);
    printk("addr: %p\n", node->addr);
    printk("size: %p\n", node->size);
    printk("used: %p\n", node->used);
}

void simple_mm_info(void)
{
    mm_node_t *node;

    for (node = head_node; node != NULL; node = node->next)
    {
        node_dump(node, __func__);
    }
}

/**
 * @brief 初始化内存分配算法
 *        使用 mm_node_t 来管理内存片段
 *        将该结构体放在内存空间的头部
 *        使用链表串联起来全部的内存片段
 *
 *        之后所有申请到的内存空间都在下面参数定义的区域
 * @param start_addr 动态内存分配管理空间起始地址
 * @param size 动态内存分配管理空间大小
 * @return int
 */
int mm_init(size_t start_addr, size_t size)
{
    //初始化内存管理节点与算法
    //内存管理的节点在内存开始位置
    head_node = (mm_node_t *)start_addr;

    //初始化默认节点的内存管理的参数
    head_node->addr = start_addr + SIZEOF_MM_NODE;
    head_node->size = size - SIZEOF_MM_NODE;
    head_node->prev = NULL;
    head_node->next = NULL;
    head_node->used = 0;

    node_dump(head_node, __func__);
}

/**
 * @brief 申请指定大小内存空间
 *
 * @param size
 * @return void*
 */
void *kmalloc(size_t size)
{
    mm_node_t *node;
    mm_node_t *new_node;
    uint32_t align = 8;

    void *ptr = NULL;

    if (size == 0)
        return NULL;

    /**
     * @brief 分配需要适当的对齐
     *        如果申请8字节的数据 但是没有对齐的话，访问会出现异常
     *
     */
    size = (((size) + (align)-1) & ~((align)-1));

    //遍历全部的节点
    for (node = head_node; node != NULL; node = node->next)
    {
        node_dump(node, __func__);

        //如果当前节点的大小与申请内存大小相同 修改该节点的标记之后返回节点地址
        if (size == node->size && node->used == 0)
        {
            node->used = 1;
            ptr = (void *)node->addr;
            break;
        }

        //如果申请内存大小小于当前节点的大小 拆分当前节点为2个节点
        //把前一个节点标记为已经使用，把剩余空间信息更新到第二个节点
        //返回节点的地址
        if (size < (node->size - SIZEOF_MM_NODE) && node->used == 0)
        {
            new_node = (mm_node_t *)(node->addr + size);

            new_node->addr = (uint64_t)((uint64_t)new_node + SIZEOF_MM_NODE);
            new_node->size = node->size - size - SIZEOF_MM_NODE;
            new_node->used = 0;
            new_node->next = node->next;
            new_node->prev = node;

            node->size = size;
            node->used = 1;
            node->next = new_node;

            ptr = (void *)node->addr;
            break;
        }
    }

    return ptr;
}

/**
 * @brief 释放指定的内存片段
 *
 * @param ptr
 */
void kfree(void *ptr)
{
    mm_node_t *node;
    mm_node_t *new_node;

    if (ptr == 0)
        return;

    //通过内存空间地址找到内存片段管理节点
    node = (mm_node_t *)((uint64_t)ptr - SIZEOF_MM_NODE);
    node_dump(node, __func__);

    //标记内存节点信息为已经释放
    node->used = 0;

    //进一步回收 与前节点、后节点合并（单个节点越大 越有利于申请 减少碎片化）

    //若前节点与后节点都是释放状态，则合并三个节点
    if (node->prev != NULL && node->prev->used == 0 && node->next != NULL && node->next->used == 0)
    {
        node->prev->next = node->next->next;
        node->next->next->prev = node->prev;
        node->prev->size += node->size + SIZEOF_MM_NODE + node->next->size + SIZEOF_MM_NODE;
    }

    //若前节点是已经释放 与前节点合并
    if (node->prev != NULL && node->prev->used == 0)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        node->prev->size += node->size + SIZEOF_MM_NODE;
    }

    //若后节点释放 与后节点合并
    if (node->next != NULL && node->next->used == 0)
    {
        node->size += node->next->size + SIZEOF_MM_NODE;

        if (node->next->next != NULL)
            node->next->next->prev = node;

        node->next = node->next->next;
    }
}

int mm_test_case(void)
{
    mm_init(0x50000000, 0x10000000);
    int i = 10;
    void *addr[10];

    for (i = 0; i < 10; i++)
    {
        addr[i] = kmalloc(i);
        printk("request: %p\n", addr);
    }

    kfree(addr[2]);
    kfree(addr[3]);
    kfree(addr[4]);

    simple_mm_info();
}