#ifndef __TICK_H__
#define __TICK_H__

#include <stdint.h>

#define TICK_PER_SECOND (100)

void task_sleep_ms(uint64_t ms);

#endif // !__TICK_H__
