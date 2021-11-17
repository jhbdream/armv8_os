#include <stdio.h>
#include <board_init.h>
#include <elog.h>
#include <config.h>
#include <driver/gic.h>
#include <arch_timer.h>
#include <common/interrupt.h>

/**
 * @brief 初始化elog 日志库
 *
 */
void elog_lib_init(void)
{
    /* 关闭行缓冲 */
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_TIME);

#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif

    /* start EasyLogger */
    elog_start();
}

/**
 * EasyLogger demo
 */
void test_elog(void)
{
    /* test log output for all level */
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
    elog_raw("Hello EasyLogger!\n");
}

/**
 * @brief 关闭全局中断
 *        初始化中断控制器(GIC V3)
 *
 * @return int
 */
int interrupt_init(void)
{
    local_irq_disable();
    gic_init();
}
