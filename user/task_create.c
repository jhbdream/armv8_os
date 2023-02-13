#include <kernel/task.h>
#include <kernel/tick.h>
#include <common/delay.h>
#include <stddef.h>
#include <printk.h>

extern struct task taska;
extern struct task taskb;

/**
 * @brief taska
 *
 */
struct task taska;
char taska_stack[4096 * 8];

void taska_fun(void)
{
	while (1) {
		task_sleep_ms(1000);
	}
}

/**
 * @brief taskb
 *
 */
struct task taskb;
char taskb_stack[4096 * 8];

void taskb_fun(void)
{
	while (1) {
		task_sleep_ms(500);
	}
}

char task_idle_stack[4096];
static uint64_t idel_count = 0;
void task_idle(void)
{
	while (1) {
		idel_count++;
	}
}

/**
 * @brief init create task and switch to taska
 *
 */
void user_task_init(void)
{
	struct task *taskp;

	taskp = task_create("taskA", taska_stack + sizeof(taska_stack),
			    taska_fun, 20);
	task_create("taskB", taskb_stack + sizeof(taskb_stack), taskb_fun, 21);
	task_create("idle", task_idle_stack + sizeof(task_idle_stack),
		    task_idle, 0);

	task_switch_to(taskp);
}
