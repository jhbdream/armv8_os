#include <kernel/semaphore.h>

int sem_init(struct semaphore *sem)
{
    sem->wait_task = NULL;
    sem->value = 0;
}

int sem_take(struct semaphore *sem)
{

    if(sem->value > 0)
    {
        sem->value = sem->value - 1;
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

    sem->value = sem->value + 1;

    t = sem->wait_task;
    if(t != NULL)
    {
        t->task_state = TASK_STATE_READY;
    }
    schedle();

    return 0;
}