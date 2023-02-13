#include <eeos.h>
#include <asm/csr.h>
#include <io.h>

#if 1
/* C910 */
#define TIMECMP_BASE 0x64004000
#else
/* qemu */
#define TIMECMP_BASE 0x02004000
#endif

static u64 get_ticks(void)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	return n;
}

void riscv_timer_init(void)
{
	u64 val = -1ULL;
	u32 mask = -1U;
	void *addr;

	/* init compare val */
	addr = (void *)(TIMECMP_BASE);
	writel(val & mask, addr);
	writel(val >> 32, addr + 4);

	/* enable timer irq */
	csr_set(mie, MIE_MTIE);
}
