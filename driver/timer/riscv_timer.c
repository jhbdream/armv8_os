#include <eeos.h>
#include <asm/csr.h>

static u64 get_ticks(void)
{
	unsigned long n;

	__asm__ __volatile__("rdtime %0" : "=r"(n));
	return n;
}

void riscv_timer_start(void)
{
	csr_set(mie, MIE_MTIE);
}

