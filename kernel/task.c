#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <eeos/irqflags.h>

#include <kernel/task.h>

#define TASK_COUNT (32)

struct gp_regs {
    uint64_t regs[31]; // 31 个通用寄存器
    uint64_t sp;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};

static struct task task_table[TASK_COUNT] = { 0 };

/**
 * @brief 通过 tid 判断任务是否空闲 分配空闲任务
 *
 * @return
 */
static struct task *alloc_task(void)
{
    int i;

    struct task *p;

    for (i = 0; i < TASK_COUNT; i++) {
        p = &task_table[i];

        if (p->state == TASK_STATE_FREE) {
            p->state = TASK_STATE_INIT;
            return p;
        }
    }

    return NULL;
}

struct task *create_task(char *name, task_func_t func, void *stack_base,
                         uint32_t stack_size, uint32_t flags)
{
    int i;

    struct task *p;

    struct gp_regs *regs;

    if (name == NULL) {
        return NULL;
    }

    if (func == NULL) {
        return NULL;
    }

    p = alloc_task();
    if (p == NULL) {
        return NULL;
    }

    strncpy(p->name, name, TASK_NAME_SIZE);

    p->stack_bottom = stack_base;
    p->stack_top    = stack_base + stack_size;
    p->stack_base   = p->stack_top;

    regs = p->stack_base - sizeof(struct gp_regs);

    for (i = 0; i < 31; i++) {
        regs->regs[i] = i;
    }

    regs->sp       = (uint64_t)p->stack_base;
    regs->spsr_el1 = (uint64_t)0x345;
    regs->elr_el1  = (uint64_t)func;

    p->stack_base = regs;
    p->state      = TASK_STATE_READY;

    return NULL;
}
