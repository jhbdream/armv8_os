#include <kernel/semaphore.h>
#include <common/interrupt.h>
#include <stddef.h>

int sem_init(struct semaphore *sem, uint32_t count)
{
	sem->wait_task = NULL;
	sem->value = count;
}

int sem_take(struct semaphore *sem)
{
	uint64_t flags;
	if (sem->value > 0) {
		local_irq_save(flags);
		sem->value = sem->value - 1;
		local_irq_restore(flags);
	} else {
		struct task *t;

		t = g_current_task;
		sem->wait_task = t;
		t->task_state = TASK_STATE_WAIT_SEM;

		if (is_interrupt_nest()) {
			schedle_interrupt();
		} else {
			schedle();
		}
	}
}

int sem_release(struct semaphore *sem)
{
	struct task *t;
	uint64_t flags;

	local_irq_save(flags);
	sem->value = sem->value + 1;
	local_irq_restore(flags);

	t = sem->wait_task;
	if (t != NULL) {
		t->task_state = TASK_STATE_READY;
		if (is_interrupt_nest()) {
			schedle_interrupt();
		} else {
			schedle();
		}
	}

	return 0;
}