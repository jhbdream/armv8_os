#ifndef __TICK_H__
#define __TICK_H__
#include <ee_stdint.h>

extern volatile uint64_t g_systic;

#define TICK_PER_SECOND (100)

void task_sleep_ms(uint64_t ms);

#endif // !__TICK_H__
