#include <common/delay.h>

void ndelay(uint32_t ns)
{
#if 0
    uint64_t current = arch_timer_read();
    uint64_t condition = current + arch_timer_frequecy() * ns / 1000000000;

    while (condition > current)
    {
        current = arch_timer_read();
    }
#endif
}

void udelay(uint32_t us)
{
#if 0
    uint64_t current = arch_timer_read();
    uint64_t condition = current + arch_timer_frequecy() * us / 1000000;

    while (condition > current)
    {
        current = arch_timer_read();
    }
#endif
}

void mdelay(uint32_t ms)
{
#if 0
	uint64_t current = arch_timer_read();
    uint64_t condition = current + arch_timer_frequecy() * ms / 1000;

    while (condition > current)
    {
        current = arch_timer_read();
    }
#endif
}
