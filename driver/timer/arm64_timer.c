#include <stddef.h>
#include <stdint.h>
#include <printk.h>
#include <kernel/task.h>
#include <driver/interrupt.h>

uint64_t cpu_khz = 0;

static inline uint64_t read_cntpct_el0(void)
{
    uint64_t cnt;
    /*
     * MRS 指令将系统寄存器的值读入通用寄存器
     * “cntpct_el0” 是物理计数器（Physical Count）的 64 位寄存器
     */
    asm volatile("mrs %0, cntpct_el0"
                 : "=r"(cnt) /* output: cnt 由通用寄存器返回 */
                 :           /* no inputs */
                 :           /* no clobbers besides gcc 认为会改的那些 */
    );

    return cnt;
}

static inline uint64_t read_cntfrq_el0(void)
{
    uint64_t cnt;

    asm volatile("mrs %0, cntfrq_el0"
                 : "=r"(cnt) /* output: cnt 由通用寄存器返回 */
                 :           /* no inputs */
                 :           /* no clobbers besides gcc 认为会改的那些 */
    );

    return cnt;
}

#include <stdint.h>

/* 读 CNTPS_CTL_EL0 */
static inline uint64_t read_cntp_ctl_el0(void)
{
    uint64_t val;
    asm volatile("mrs %0, cntp_ctl_el0" : "=r"(val));
    return val;
}

/* 写 CNTPS_CTL_EL0 */
static inline void write_cntp_ctl_el0(uint64_t val)
{
    asm volatile("msr cntp_ctl_el0, %0" ::"r"(val));
    /* 确保寄存器写入生效 */
    asm volatile("isb" ::: "memory");
}

static inline void write_cntp_tval_el0(uint64_t val)
{
    asm volatile("msr cntp_tval_el0, %0" ::"r"(val));
    /* 确保寄存器写入生效 */
    asm volatile("isb" ::: "memory");
}

/* 启动物理定时器 */
static inline void arch_timer_start(void)
{
    uint64_t ctrl = read_cntp_ctl_el0();
    if (!(ctrl & 0x1)) {
        ctrl |= 0x1;
        write_cntp_ctl_el0(ctrl);
    }
}

/* 停止物理定时器 */
static inline void arch_timer_stop(void)
{
    uint64_t ctrl = read_cntp_ctl_el0();
    if (ctrl & 0x1) {
        ctrl &= ~0x1;
        write_cntp_ctl_el0(ctrl);
    }
}

/* 使能物理定时器中断（清除屏蔽位）*/
static inline void arch_timer_interrupt_enable(void)
{
    uint64_t ctrl = read_cntp_ctl_el0();
    /* bit[1]=1 表示屏蔽中断，写 0 以取消屏蔽 */
    if (ctrl & (1UL << 1)) {
        ctrl &= ~(1UL << 1);
        write_cntp_ctl_el0(ctrl);
    }
}

/* 禁止物理定时器中断（设置屏蔽位）*/
static inline void arch_timer_interrupt_disable(void)
{
    uint64_t ctrl = read_cntp_ctl_el0();
    if (!(ctrl & (1UL << 1))) {
        ctrl |= (1UL << 1);
        write_cntp_ctl_el0(ctrl);
    }
}

void arm64_arch_timer_tandler(struct irq_desc *desc)
{
    schedle_interrupt();
    write_cntp_tval_el0(cpu_khz * 10);
}

void arm64_arch_timer_init(void)
{
    uint64_t boot_tick;

    boot_tick = read_cntpct_el0();
    cpu_khz   = read_cntfrq_el0() / 1000;

    printk("boot ticks : [ 0x%llx ]\n", boot_tick);
    printk("clock freq : [ %dKhz ]\n", cpu_khz);

    arch_timer_interrupt_disable();
    arch_timer_stop();

    if (request_irq(30, arm64_arch_timer_tandler, 0, "arch_timer", NULL) < 0) {
        return;
    }

    write_cntp_tval_el0(cpu_khz * 10);
    arch_timer_start();

    arch_timer_interrupt_enable();
}
