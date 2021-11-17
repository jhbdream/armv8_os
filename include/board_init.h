#ifndef __BOARD_INIT_H__
#define __BOARD_INIT_H__

void user_task_init(void);
void systic_timer_init(void);
int interrupt_init(void);
void elog_lib_init(void);

#endif