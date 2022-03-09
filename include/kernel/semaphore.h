#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <printk.h>
#include <kernel/task.h>

struct semaphore
{
    struct task *wait_task;
    uint32_t value;
};

int sem_init(struct semaphore *sem, uint32_t count);
int sem_take(struct semaphore *sem);
int sem_release(struct semaphore *sem);

#endif // !__SEMAPHORE_H__