#ifndef __ARCH_TIMER_H
#define __ARCH_TIMER_H

#include <stdint.h>
#include <sysreg.h>

static inline void arch_timer_start(void)
{
	uint64_t ctrl = read_sysreg(cntp_ctl_el0);
	if (!(ctrl & (1 << 0))) {
		ctrl |= (1 << 0);
		write_sysreg(ctrl, cntp_ctl_el0);
	}
}

static inline void arch_timer_stop(void)
{
	uint64_t ctrl = read_sysreg(cntp_ctl_el0);
	if ((ctrl & (1 << 0))) {
		ctrl &= ~(1 << 0);
		write_sysreg(ctrl, cntp_ctl_el0);
	}
}

static inline void arch_timer_interrupt_enable(void)
{
	uint64_t ctrl = read_sysreg(cntp_ctl_el0);
	if (ctrl & (1 << 1)) {
		ctrl &= ~(1 << 1);
		write_sysreg(ctrl, cntp_ctl_el0);
	}
}

static inline void arch_timer_interrupt_disable(void)
{
	uint64_t ctrl = read_sysreg(cntp_ctl_el0);
	if (!(ctrl & (1 << 1))) {
		ctrl |= (1 << 1);
		write_sysreg(ctrl, cntp_ctl_el0);
	}
}

static inline int arch_timer_interrupt_status(void)
{
	uint64_t ctrl = read_sysreg(cntp_ctl_el0);
	return (ctrl >> 2) & 0x01;
}

static inline uint64_t arch_timer_frequecy(void)
{
	uint64_t rate = read_sysreg(cntfrq_el0);
	return (rate != 0) ? rate : 1000000;
}

static inline uint64_t arch_timer_read(void)
{
	return read_sysreg(cntpct_el0);
}

static inline void arch_timer_compare(uint64_t interval)
{
	write_sysreg(interval, cntp_tval_el0);
}

#endif