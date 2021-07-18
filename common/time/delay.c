#include <common/delay.h>
#include <arch_timer.h>

void ndelay(uint32_t ns)
{
    uint64_t current = arch_timer_read();
    uint64_t condition = current + arch_timer_frequecy() * ns / 1000000000;

    while (condition > current)
    {
        current = arch_timer_read();
    }
}

void udelay(uint32_t us)
{
    uint64_t current = arch_timer_read();
    uint64_t condition = current + arch_timer_frequecy() * us / 1000000;

    while (condition > current)
    {
        current = arch_timer_read();
    }
}

void mdelay(uint32_t ms)
{
    uint64_t current = arch_timer_read();
    uint64_t condition = current + arch_timer_frequecy() * ms / 1000;

    while (condition > current)
    {
        current = arch_timer_read();
    }
}
