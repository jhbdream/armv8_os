#include <kernel/task.h>
#include <kernel/semaphore.h>
#include <kernel/tick.h>

#include <stdio.h>
#include <common/delay.h>
#include <elog.h>

struct semaphore sem;

struct task sema;
char sema_stack[4096 * 8];

void sema_task(void)
{

    log_i("task pid is %d!", getpid());
    log_i("i am %s run! line:[%d]!", __func__, __LINE__);

    while (1)
    {
        log_i("i am %s run! line:[%d] sem_release!", __func__, __LINE__);
        sem_release(&sem);
        task_sleep_ms(500);
    }
}

struct task semb;
char semb_stack[4096 * 8];

void semb_task(void)
{
    log_i("task pid is %d!", getpid());
    log_i("i am %s run! line:[%d]!", __func__, __LINE__);

    while (1)
    {
        sem_take(&sem);
        log_i("i am %s run! line:[%d] get sem!", __func__, __LINE__);
    }
}


void sem_task_init(void)
{
    sem_init(&sem, 1);
    task_create("sema", sema_stack + sizeof(sema_stack), sema_task, 50);
    task_create("semb", semb_stack + sizeof(semb_stack), semb_task, 51);
}