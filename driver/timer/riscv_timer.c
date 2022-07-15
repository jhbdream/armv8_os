#include <eeos.h>
#include <asm/csr.h>
#include <io.h>

#define PLIC_BASE 0x60000000
#define CLINT_OFFSET 0x04000000
#define TIME_CMP_OFFSET 0x4000

static u64 get_ticks(void)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	return n;
}

void riscv_timer_start(void)
{
	u64 val = -1ULL;
	u32 mask = -1U;
	void *addr;

	/* init compare val */
	addr = (void *)(PLIC_BASE + CLINT_OFFSET + TIME_CMP_OFFSET);
	writel(val & mask, addr);
	writel(val >> 32, addr + 4);

	/* enable timer irq */
	csr_set(mie, MIE_MTIE);
}

