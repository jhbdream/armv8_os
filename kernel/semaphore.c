#include <kernel/semaphore.h>
#include <common/interrupt.h>

int sem_init(struct semaphore *sem, uint32_t count)
{
    sem->wait_task = NULL;
    sem->value = count;
}

int sem_take(struct semaphore *sem)
{
    if(sem->value > 0)
    {
        local_irq_disable();
        sem->value = sem->value - 1;
        local_irq_enable();
    }
    else
    {
        struct task *t;

        t = g_current_task;
        sem->wait_task = t;
        t->task_state = TASK_STATE_WAIT_SEM;
        schedle();
    }
}

int sem_release(struct semaphore *sem)
{
    struct task *t;

    local_irq_disable();
    sem->value = sem->value + 1;
    local_irq_enable();

    t = sem->wait_task;
    if(t != NULL)
    {
        t->task_state = TASK_STATE_READY;
        schedle();
    }

    return 0;
}