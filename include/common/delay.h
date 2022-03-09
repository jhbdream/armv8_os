#ifndef __DELAY_H__
#define __DELAY_H__

#include <ee_stdint.h>

void ndelay(uint32_t ns);
void udelay(uint32_t us);
void mdelay(uint32_t ms);

#endif /* __DELAY_H__ */
