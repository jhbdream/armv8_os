#include <kernel/tick.h>
#include <kernel/task.h>
#include <common/interrupt.h>

volatile uint64_t g_systic = 0;

/**
 * @brief 将tick转换为ms单位
 *
 * @param tick
 * @return uint64_t
 */
uint64_t tick_to_ms(uint64_t tick)
{
	return g_systic * (1000u / TICK_PER_SECOND);
}

/**
 * @brief 将ms时间转换为tick
 *
 * @param tick
 * @return uint64_t
 */
uint64_t ms_to_tick(uint64_t ms)
{
	uint64_t tick;
	tick = TICK_PER_SECOND * (ms / 1000);
	tick += (TICK_PER_SECOND * (ms % 1000) + 999) / 1000;

	return tick;
}

/**
 * @brief 使当前任务让出处理器，休眠指定的时间之后在执行
 *
 * @param ms
 * @return int
 */
void task_sleep_ms(uint64_t ms)
{
	local_irq_disable();

	//设置睡眠任务的状态
	struct task *t = g_current_task;
	t->task_state = TASK_STATE_SLEEP;

	//设置任务的唤醒时间
	t->sleep_timeout = g_systic + ms_to_tick(ms);

	//发起调度，切出任务
	schedle();

	local_irq_enable();
}
