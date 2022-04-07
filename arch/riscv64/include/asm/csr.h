#ifndef	_ASM_RISCV_CSR_H
#define _ASM_RISCV_CSR_H

#include <const.h>

#define SR_FS       _AC(0x00006000, UL) /* Floating-point Status */

/* PMP configuration */
#define PMP_R           0x01
#define PMP_W           0x02
#define PMP_X           0x04
#define PMP_A           0x18
#define PMP_A_TOR       0x08
#define PMP_A_NA4       0x10
#define PMP_A_NAPOT     0x18
#define PMP_L           0x80

#endif
