/**
 * @file sysreg.h
 * @author jihongbin (longma@orbbec.com)
 * @brief
 * @version 0.1
 * @date 2022-05-07
 * @copyright Copyright (c) {2015-2021} Orbbec
 *
 */

#ifndef __ASM_SYSREG_H
#define __ASM_SYSREG_H

#include <bits.h>

/*
 * ARMv8 ARM reserves the following encoding for system registers:
 * (Ref: ARMv8 ARM, Section: "System instruction class encoding overview",
 *  C5.2, version:ARM DDI 0487A.f)
 *  [20-19] : Op0
 *  [18-16] : Op1
 *  [15-12] : CRn
 *  [11-8]  : CRm
 *  [7-5]   : Op2
 */
#define Op0_shift 19
#define Op0_mask 0x3
#define Op1_shift 16
#define Op1_mask 0x7
#define CRn_shift 12
#define CRn_mask 0xf
#define CRm_shift 8
#define CRm_mask 0xf
#define Op2_shift 5
#define Op2_mask 0x7

#define sys_reg(op0, op1, crn, crm, op2)                                       \
	(((op0) << Op0_shift) | ((op1) << Op1_shift) | ((crn) << CRn_shift) |  \
	 ((crm) << CRm_shift) | ((op2) << Op2_shift))

#define sys_insn sys_reg

#define sys_reg_Op0(id) (((id) >> Op0_shift) & Op0_mask)
#define sys_reg_Op1(id) (((id) >> Op1_shift) & Op1_mask)
#define sys_reg_CRn(id) (((id) >> CRn_shift) & CRn_mask)
#define sys_reg_CRm(id) (((id) >> CRm_shift) & CRm_mask)
#define sys_reg_Op2(id) (((id) >> Op2_shift) & Op2_mask)

/* Common SCTLR_ELx flags. */
#define SCTLR_ELx_DSSBS (BIT(44))
#define SCTLR_ELx_ATA (BIT(43))

#define SCTLR_ELx_TCF_SHIFT 40
#define SCTLR_ELx_TCF_NONE (UL(0x0) << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_SYNC (UL(0x1) << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_ASYNC (UL(0x2) << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_ASYMM (UL(0x3) << SCTLR_ELx_TCF_SHIFT)
#define SCTLR_ELx_TCF_MASK (UL(0x3) << SCTLR_ELx_TCF_SHIFT)

#define SCTLR_ELx_ENIA_SHIFT 31

#define SCTLR_ELx_ITFSB (BIT(37))
#define SCTLR_ELx_ENIA (BIT(SCTLR_ELx_ENIA_SHIFT))
#define SCTLR_ELx_ENIB (BIT(30))
#define SCTLR_ELx_ENDA (BIT(27))
#define SCTLR_ELx_EE (BIT(25))
#define SCTLR_ELx_IESB (BIT(21))
#define SCTLR_ELx_WXN (BIT(19))
#define SCTLR_ELx_ENDB (BIT(13))
#define SCTLR_ELx_I (BIT(12))
#define SCTLR_ELx_SA (BIT(3))
#define SCTLR_ELx_C (BIT(2))
#define SCTLR_ELx_A (BIT(1))
#define SCTLR_ELx_M (BIT(0))

/* SCTLR_EL1 specific flags. */
#define SCTLR_EL1_EPAN (BIT(57))
#define SCTLR_EL1_ATA0 (BIT(42))

#define SCTLR_EL1_TCF0_SHIFT 38
#define SCTLR_EL1_TCF0_NONE (UL(0x0) << SCTLR_EL1_TCF0_SHIFT)
#define SCTLR_EL1_TCF0_SYNC (UL(0x1) << SCTLR_EL1_TCF0_SHIFT)
#define SCTLR_EL1_TCF0_ASYNC (UL(0x2) << SCTLR_EL1_TCF0_SHIFT)
#define SCTLR_EL1_TCF0_ASYMM (UL(0x3) << SCTLR_EL1_TCF0_SHIFT)
#define SCTLR_EL1_TCF0_MASK (UL(0x3) << SCTLR_EL1_TCF0_SHIFT)

#define SCTLR_EL1_BT1 (BIT(36))
#define SCTLR_EL1_BT0 (BIT(35))
#define SCTLR_EL1_UCI (BIT(26))
#define SCTLR_EL1_E0E (BIT(24))
#define SCTLR_EL1_SPAN (BIT(23))
#define SCTLR_EL1_NTWE (BIT(18))
#define SCTLR_EL1_NTWI (BIT(16))
#define SCTLR_EL1_UCT (BIT(15))
#define SCTLR_EL1_DZE (BIT(14))
#define SCTLR_EL1_UMA (BIT(9))
#define SCTLR_EL1_SED (BIT(8))
#define SCTLR_EL1_ITD (BIT(7))
#define SCTLR_EL1_CP15BEN (BIT(5))
#define SCTLR_EL1_SA0 (BIT(4))

#define SCTLR_EL1_RES1                                                         \
	((BIT(11)) | (BIT(20)) | (BIT(22)) | (BIT(28)) | (BIT(29)))

#define ENDIAN_SET_EL1 0

#define INIT_SCTLR_EL1_MMU_OFF (ENDIAN_SET_EL1 | SCTLR_EL1_RES1)

#define INIT_SCTLR_EL1_MMU_ON                                                  \
	(SCTLR_ELx_M | SCTLR_ELx_C | SCTLR_ELx_SA | SCTLR_EL1_SA0 |            \
	 SCTLR_EL1_SED | SCTLR_ELx_I | SCTLR_EL1_DZE | SCTLR_EL1_UCT |         \
	 SCTLR_EL1_NTWE | SCTLR_ELx_IESB | SCTLR_EL1_SPAN | SCTLR_ELx_ITFSB |  \
	 ENDIAN_SET_EL1 | SCTLR_EL1_UCI | SCTLR_EL1_EPAN | SCTLR_EL1_RES1)

/* MAIR_ELx memory attributes (used by Linux) */
#define MAIR_ATTR_DEVICE_nGnRnE UL(0x00)
#define MAIR_ATTR_DEVICE_nGnRE UL(0x04)
#define MAIR_ATTR_NORMAL_NC UL(0x44)
#define MAIR_ATTR_NORMAL_TAGGED UL(0xf0)
#define MAIR_ATTR_NORMAL UL(0xff)
#define MAIR_ATTR_MASK UL(0xff)

/* Position the attr at the correct index */
#define MAIR_ATTRIDX(attr, idx) ((attr) << ((idx)*8))

#endif