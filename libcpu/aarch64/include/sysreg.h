/*
 * Macros for accessing system registers with older binutils.
 *
 * Copyright (C) 2014 ARM Ltd.
 * Author: Catalin Marinas <catalin.marinas@arm.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ASM_SYSREG_H
#define __ASM_SYSREG_H

#include <stdint.h>
#include <stringify.h>

/*
 * ARMv8 ARM reserves the following encoding for system registers:
 * (Ref: ARMv8 ARM, Section: "System instruction class encoding overview",
 *  C5.2, version:ARM DDI 0487A.f)
 *	[20-19] : Op0
 *	[18-16] : Op1
 *	[15-12] : CRn
 *	[11-8]  : CRm
 *	[7-5]   : Op2
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

#define sys_reg(op0, op1, crn, crm, op2)           \
    (((op0) << Op0_shift) | ((op1) << Op1_shift) | \
     ((crn) << CRn_shift) | ((crm) << CRm_shift) | \
     ((op2) << Op2_shift))

#define sys_insn sys_reg

#define sys_reg_Op0(id) (((id) >> Op0_shift) & Op0_mask)
#define sys_reg_Op1(id) (((id) >> Op1_shift) & Op1_mask)
#define sys_reg_CRn(id) (((id) >> CRn_shift) & CRn_mask)
#define sys_reg_CRm(id) (((id) >> CRm_shift) & CRm_mask)
#define sys_reg_Op2(id) (((id) >> Op2_shift) & Op2_mask)

#define SYS_DC_ISW sys_insn(1, 0, 7, 6, 2)
#define SYS_DC_CSW sys_insn(1, 0, 7, 10, 2)
#define SYS_DC_CISW sys_insn(1, 0, 7, 14, 2)

#define SYS_OSDTRRX_EL1 sys_reg(2, 0, 0, 0, 2)
#define SYS_MDCCINT_EL1 sys_reg(2, 0, 0, 2, 0)
#define SYS_MDSCR_EL1 sys_reg(2, 0, 0, 2, 2)
#define SYS_OSDTRTX_EL1 sys_reg(2, 0, 0, 3, 2)
#define SYS_OSECCR_EL1 sys_reg(2, 0, 0, 6, 2)
#define SYS_DBGBVRn_EL1(n) sys_reg(2, 0, 0, n, 4)
#define SYS_DBGBCRn_EL1(n) sys_reg(2, 0, 0, n, 5)
#define SYS_DBGWVRn_EL1(n) sys_reg(2, 0, 0, n, 6)
#define SYS_DBGWCRn_EL1(n) sys_reg(2, 0, 0, n, 7)
#define SYS_MDRAR_EL1 sys_reg(2, 0, 1, 0, 0)
#define SYS_OSLAR_EL1 sys_reg(2, 0, 1, 0, 4)
#define SYS_OSLSR_EL1 sys_reg(2, 0, 1, 1, 4)
#define SYS_OSDLR_EL1 sys_reg(2, 0, 1, 3, 4)
#define SYS_DBGPRCR_EL1 sys_reg(2, 0, 1, 4, 4)
#define SYS_DBGCLAIMSET_EL1 sys_reg(2, 0, 7, 8, 6)
#define SYS_DBGCLAIMCLR_EL1 sys_reg(2, 0, 7, 9, 6)
#define SYS_DBGAUTHSTATUS_EL1 sys_reg(2, 0, 7, 14, 6)
#define SYS_MDCCSR_EL0 sys_reg(2, 3, 0, 1, 0)
#define SYS_DBGDTR_EL0 sys_reg(2, 3, 0, 4, 0)
#define SYS_DBGDTRRX_EL0 sys_reg(2, 3, 0, 5, 0)
#define SYS_DBGDTRTX_EL0 sys_reg(2, 3, 0, 5, 0)
#define SYS_DBGVCR32_EL2 sys_reg(2, 4, 0, 7, 0)

#define SYS_MIDR_EL1 sys_reg(3, 0, 0, 0, 0)
#define SYS_MPIDR_EL1 sys_reg(3, 0, 0, 0, 5)
#define SYS_REVIDR_EL1 sys_reg(3, 0, 0, 0, 6)

#define SYS_ID_PFR0_EL1 sys_reg(3, 0, 0, 1, 0)
#define SYS_ID_PFR1_EL1 sys_reg(3, 0, 0, 1, 1)
#define SYS_ID_DFR0_EL1 sys_reg(3, 0, 0, 1, 2)
#define SYS_ID_AFR0_EL1 sys_reg(3, 0, 0, 1, 3)
#define SYS_ID_MMFR0_EL1 sys_reg(3, 0, 0, 1, 4)
#define SYS_ID_MMFR1_EL1 sys_reg(3, 0, 0, 1, 5)
#define SYS_ID_MMFR2_EL1 sys_reg(3, 0, 0, 1, 6)
#define SYS_ID_MMFR3_EL1 sys_reg(3, 0, 0, 1, 7)

#define SYS_ID_ISAR0_EL1 sys_reg(3, 0, 0, 2, 0)
#define SYS_ID_ISAR1_EL1 sys_reg(3, 0, 0, 2, 1)
#define SYS_ID_ISAR2_EL1 sys_reg(3, 0, 0, 2, 2)
#define SYS_ID_ISAR3_EL1 sys_reg(3, 0, 0, 2, 3)
#define SYS_ID_ISAR4_EL1 sys_reg(3, 0, 0, 2, 4)
#define SYS_ID_ISAR5_EL1 sys_reg(3, 0, 0, 2, 5)
#define SYS_ID_MMFR4_EL1 sys_reg(3, 0, 0, 2, 6)

#define SYS_MVFR0_EL1 sys_reg(3, 0, 0, 3, 0)
#define SYS_MVFR1_EL1 sys_reg(3, 0, 0, 3, 1)
#define SYS_MVFR2_EL1 sys_reg(3, 0, 0, 3, 2)

#define SYS_ID_AA64PFR0_EL1 sys_reg(3, 0, 0, 4, 0)
#define SYS_ID_AA64PFR1_EL1 sys_reg(3, 0, 0, 4, 1)
#define SYS_ID_AA64ZFR0_EL1 sys_reg(3, 0, 0, 4, 4)

#define SYS_ID_AA64DFR0_EL1 sys_reg(3, 0, 0, 5, 0)
#define SYS_ID_AA64DFR1_EL1 sys_reg(3, 0, 0, 5, 1)

#define SYS_ID_AA64AFR0_EL1 sys_reg(3, 0, 0, 5, 4)
#define SYS_ID_AA64AFR1_EL1 sys_reg(3, 0, 0, 5, 5)

#define SYS_ID_AA64ISAR0_EL1 sys_reg(3, 0, 0, 6, 0)
#define SYS_ID_AA64ISAR1_EL1 sys_reg(3, 0, 0, 6, 1)

#define SYS_ID_AA64MMFR0_EL1 sys_reg(3, 0, 0, 7, 0)
#define SYS_ID_AA64MMFR1_EL1 sys_reg(3, 0, 0, 7, 1)
#define SYS_ID_AA64MMFR2_EL1 sys_reg(3, 0, 0, 7, 2)

#define SYS_SCTLR_EL1 sys_reg(3, 0, 1, 0, 0)
#define SYS_ACTLR_EL1 sys_reg(3, 0, 1, 0, 1)
#define SYS_CPACR_EL1 sys_reg(3, 0, 1, 0, 2)

#define SYS_ZCR_EL1 sys_reg(3, 0, 1, 2, 0)

#define SYS_TTBR0_EL1 sys_reg(3, 0, 2, 0, 0)
#define SYS_TTBR1_EL1 sys_reg(3, 0, 2, 0, 1)
#define SYS_TCR_EL1 sys_reg(3, 0, 2, 0, 2)

#define SYS_ICC_PMR_EL1 sys_reg(3, 0, 4, 6, 0)

#define SYS_AFSR0_EL1 sys_reg(3, 0, 5, 1, 0)
#define SYS_AFSR1_EL1 sys_reg(3, 0, 5, 1, 1)
#define SYS_ESR_EL1 sys_reg(3, 0, 5, 2, 0)

#define SYS_ERRIDR_EL1 sys_reg(3, 0, 5, 3, 0)
#define SYS_ERRSELR_EL1 sys_reg(3, 0, 5, 3, 1)
#define SYS_ERXFR_EL1 sys_reg(3, 0, 5, 4, 0)
#define SYS_ERXCTLR_EL1 sys_reg(3, 0, 5, 4, 1)
#define SYS_ERXSTATUS_EL1 sys_reg(3, 0, 5, 4, 2)
#define SYS_ERXADDR_EL1 sys_reg(3, 0, 5, 4, 3)
#define SYS_ERXMISC0_EL1 sys_reg(3, 0, 5, 5, 0)
#define SYS_ERXMISC1_EL1 sys_reg(3, 0, 5, 5, 1)

#define SYS_FAR_EL1 sys_reg(3, 0, 6, 0, 0)
#define SYS_PAR_EL1 sys_reg(3, 0, 7, 4, 0)

/*** Statistical Profiling Extension ***/
/* ID registers */
#define SYS_PMSIDR_EL1 sys_reg(3, 0, 9, 9, 7)
#define SYS_PMSIDR_EL1_FE_SHIFT 0
#define SYS_PMSIDR_EL1_FT_SHIFT 1
#define SYS_PMSIDR_EL1_FL_SHIFT 2
#define SYS_PMSIDR_EL1_ARCHINST_SHIFT 3
#define SYS_PMSIDR_EL1_LDS_SHIFT 4
#define SYS_PMSIDR_EL1_ERND_SHIFT 5
#define SYS_PMSIDR_EL1_INTERVAL_SHIFT 8
#define SYS_PMSIDR_EL1_INTERVAL_MASK 0xfUL
#define SYS_PMSIDR_EL1_MAXSIZE_SHIFT 12
#define SYS_PMSIDR_EL1_MAXSIZE_MASK 0xfUL
#define SYS_PMSIDR_EL1_COUNTSIZE_SHIFT 16
#define SYS_PMSIDR_EL1_COUNTSIZE_MASK 0xfUL

#define SYS_PMBIDR_EL1 sys_reg(3, 0, 9, 10, 7)
#define SYS_PMBIDR_EL1_ALIGN_SHIFT 0
#define SYS_PMBIDR_EL1_ALIGN_MASK 0xfU
#define SYS_PMBIDR_EL1_P_SHIFT 4
#define SYS_PMBIDR_EL1_F_SHIFT 5

/* Sampling controls */
#define SYS_PMSCR_EL1 sys_reg(3, 0, 9, 9, 0)
#define SYS_PMSCR_EL1_E0SPE_SHIFT 0
#define SYS_PMSCR_EL1_E1SPE_SHIFT 1
#define SYS_PMSCR_EL1_CX_SHIFT 3
#define SYS_PMSCR_EL1_PA_SHIFT 4
#define SYS_PMSCR_EL1_TS_SHIFT 5
#define SYS_PMSCR_EL1_PCT_SHIFT 6

#define SYS_PMSCR_EL2 sys_reg(3, 4, 9, 9, 0)
#define SYS_PMSCR_EL2_E0HSPE_SHIFT 0
#define SYS_PMSCR_EL2_E2SPE_SHIFT 1
#define SYS_PMSCR_EL2_CX_SHIFT 3
#define SYS_PMSCR_EL2_PA_SHIFT 4
#define SYS_PMSCR_EL2_TS_SHIFT 5
#define SYS_PMSCR_EL2_PCT_SHIFT 6

#define SYS_PMSICR_EL1 sys_reg(3, 0, 9, 9, 2)

#define SYS_PMSIRR_EL1 sys_reg(3, 0, 9, 9, 3)
#define SYS_PMSIRR_EL1_RND_SHIFT 0
#define SYS_PMSIRR_EL1_INTERVAL_SHIFT 8
#define SYS_PMSIRR_EL1_INTERVAL_MASK 0xffffffUL

/* Filtering controls */
#define SYS_PMSFCR_EL1 sys_reg(3, 0, 9, 9, 4)
#define SYS_PMSFCR_EL1_FE_SHIFT 0
#define SYS_PMSFCR_EL1_FT_SHIFT 1
#define SYS_PMSFCR_EL1_FL_SHIFT 2
#define SYS_PMSFCR_EL1_B_SHIFT 16
#define SYS_PMSFCR_EL1_LD_SHIFT 17
#define SYS_PMSFCR_EL1_ST_SHIFT 18

#define SYS_PMSEVFR_EL1 sys_reg(3, 0, 9, 9, 5)
#define SYS_PMSEVFR_EL1_RES0 0x0000ffff00ff0f55UL

#define SYS_PMSLATFR_EL1 sys_reg(3, 0, 9, 9, 6)
#define SYS_PMSLATFR_EL1_MINLAT_SHIFT 0

/* Buffer controls */
#define SYS_PMBLIMITR_EL1 sys_reg(3, 0, 9, 10, 0)
#define SYS_PMBLIMITR_EL1_E_SHIFT 0
#define SYS_PMBLIMITR_EL1_FM_SHIFT 1
#define SYS_PMBLIMITR_EL1_FM_MASK 0x3UL
#define SYS_PMBLIMITR_EL1_FM_STOP_IRQ (0 << SYS_PMBLIMITR_EL1_FM_SHIFT)

#define SYS_PMBPTR_EL1 sys_reg(3, 0, 9, 10, 1)

/* Buffer error reporting */
#define SYS_PMBSR_EL1 sys_reg(3, 0, 9, 10, 3)
#define SYS_PMBSR_EL1_COLL_SHIFT 16
#define SYS_PMBSR_EL1_S_SHIFT 17
#define SYS_PMBSR_EL1_EA_SHIFT 18
#define SYS_PMBSR_EL1_DL_SHIFT 19
#define SYS_PMBSR_EL1_EC_SHIFT 26
#define SYS_PMBSR_EL1_EC_MASK 0x3fUL

#define SYS_PMBSR_EL1_EC_BUF (0x0UL << SYS_PMBSR_EL1_EC_SHIFT)
#define SYS_PMBSR_EL1_EC_FAULT_S1 (0x24UL << SYS_PMBSR_EL1_EC_SHIFT)
#define SYS_PMBSR_EL1_EC_FAULT_S2 (0x25UL << SYS_PMBSR_EL1_EC_SHIFT)

#define SYS_PMBSR_EL1_FAULT_FSC_SHIFT 0
#define SYS_PMBSR_EL1_FAULT_FSC_MASK 0x3fUL

#define SYS_PMBSR_EL1_BUF_BSC_SHIFT 0
#define SYS_PMBSR_EL1_BUF_BSC_MASK 0x3fUL

#define SYS_PMBSR_EL1_BUF_BSC_FULL (0x1UL << SYS_PMBSR_EL1_BUF_BSC_SHIFT)

/*** End of Statistical Profiling Extension ***/

#define SYS_PMINTENSET_EL1 sys_reg(3, 0, 9, 14, 1)
#define SYS_PMINTENCLR_EL1 sys_reg(3, 0, 9, 14, 2)

#define SYS_MAIR_EL1 sys_reg(3, 0, 10, 2, 0)
#define SYS_AMAIR_EL1 sys_reg(3, 0, 10, 3, 0)

#define SYS_LORSA_EL1 sys_reg(3, 0, 10, 4, 0)
#define SYS_LOREA_EL1 sys_reg(3, 0, 10, 4, 1)
#define SYS_LORN_EL1 sys_reg(3, 0, 10, 4, 2)
#define SYS_LORC_EL1 sys_reg(3, 0, 10, 4, 3)
#define SYS_LORID_EL1 sys_reg(3, 0, 10, 4, 7)

#define SYS_VBAR_EL1 sys_reg(3, 0, 12, 0, 0)
#define SYS_DISR_EL1 sys_reg(3, 0, 12, 1, 1)

#define SYS_ICC_IAR0_EL1 sys_reg(3, 0, 12, 8, 0)
#define SYS_ICC_EOIR0_EL1 sys_reg(3, 0, 12, 8, 1)
#define SYS_ICC_HPPIR0_EL1 sys_reg(3, 0, 12, 8, 2)
#define SYS_ICC_BPR0_EL1 sys_reg(3, 0, 12, 8, 3)
#define SYS_ICC_AP0Rn_EL1(n) sys_reg(3, 0, 12, 8, 4 | n)
#define SYS_ICC_AP0R0_EL1 SYS_ICC_AP0Rn_EL1(0)
#define SYS_ICC_AP0R1_EL1 SYS_ICC_AP0Rn_EL1(1)
#define SYS_ICC_AP0R2_EL1 SYS_ICC_AP0Rn_EL1(2)
#define SYS_ICC_AP0R3_EL1 SYS_ICC_AP0Rn_EL1(3)
#define SYS_ICC_AP1Rn_EL1(n) sys_reg(3, 0, 12, 9, n)
#define SYS_ICC_AP1R0_EL1 SYS_ICC_AP1Rn_EL1(0)
#define SYS_ICC_AP1R1_EL1 SYS_ICC_AP1Rn_EL1(1)
#define SYS_ICC_AP1R2_EL1 SYS_ICC_AP1Rn_EL1(2)
#define SYS_ICC_AP1R3_EL1 SYS_ICC_AP1Rn_EL1(3)
#define SYS_ICC_DIR_EL1 sys_reg(3, 0, 12, 11, 1)
#define SYS_ICC_RPR_EL1 sys_reg(3, 0, 12, 11, 3)
#define SYS_ICC_SGI1R_EL1 sys_reg(3, 0, 12, 11, 5)
#define SYS_ICC_ASGI1R_EL1 sys_reg(3, 0, 12, 11, 6)
#define SYS_ICC_SGI0R_EL1 sys_reg(3, 0, 12, 11, 7)
#define SYS_ICC_IAR1_EL1 sys_reg(3, 0, 12, 12, 0)
#define SYS_ICC_EOIR1_EL1 sys_reg(3, 0, 12, 12, 1)
#define SYS_ICC_HPPIR1_EL1 sys_reg(3, 0, 12, 12, 2)
#define SYS_ICC_BPR1_EL1 sys_reg(3, 0, 12, 12, 3)
#define SYS_ICC_CTLR_EL1 sys_reg(3, 0, 12, 12, 4)
#define SYS_ICC_SRE_EL1 sys_reg(3, 0, 12, 12, 5)
#define SYS_ICC_IGRPEN0_EL1 sys_reg(3, 0, 12, 12, 6)
#define SYS_ICC_IGRPEN1_EL1 sys_reg(3, 0, 12, 12, 7)

#define SYS_CONTEXTIDR_EL1 sys_reg(3, 0, 13, 0, 1)
#define SYS_TPIDR_EL1 sys_reg(3, 0, 13, 0, 4)

#define SYS_CNTKCTL_EL1 sys_reg(3, 0, 14, 1, 0)

#define SYS_CLIDR_EL1 sys_reg(3, 1, 0, 0, 1)
#define SYS_AIDR_EL1 sys_reg(3, 1, 0, 0, 7)

#define SYS_CSSELR_EL1 sys_reg(3, 2, 0, 0, 0)

#define SYS_CTR_EL0 sys_reg(3, 3, 0, 0, 1)
#define SYS_DCZID_EL0 sys_reg(3, 3, 0, 0, 7)

#define SYS_PMCR_EL0 sys_reg(3, 3, 9, 12, 0)
#define SYS_PMCNTENSET_EL0 sys_reg(3, 3, 9, 12, 1)
#define SYS_PMCNTENCLR_EL0 sys_reg(3, 3, 9, 12, 2)
#define SYS_PMOVSCLR_EL0 sys_reg(3, 3, 9, 12, 3)
#define SYS_PMSWINC_EL0 sys_reg(3, 3, 9, 12, 4)
#define SYS_PMSELR_EL0 sys_reg(3, 3, 9, 12, 5)
#define SYS_PMCEID0_EL0 sys_reg(3, 3, 9, 12, 6)
#define SYS_PMCEID1_EL0 sys_reg(3, 3, 9, 12, 7)
#define SYS_PMCCNTR_EL0 sys_reg(3, 3, 9, 13, 0)
#define SYS_PMXEVTYPER_EL0 sys_reg(3, 3, 9, 13, 1)
#define SYS_PMXEVCNTR_EL0 sys_reg(3, 3, 9, 13, 2)
#define SYS_PMUSERENR_EL0 sys_reg(3, 3, 9, 14, 0)
#define SYS_PMOVSSET_EL0 sys_reg(3, 3, 9, 14, 3)

#define SYS_TPIDR_EL0 sys_reg(3, 3, 13, 0, 2)
#define SYS_TPIDRRO_EL0 sys_reg(3, 3, 13, 0, 3)

#define SYS_CNTFRQ_EL0 sys_reg(3, 3, 14, 0, 0)

#define SYS_CNTPCT_EL0 sys_reg(3, 3, 14, 0, 1)
#define SYS_CNTP_TVAL_EL0 sys_reg(3, 3, 14, 2, 0)
#define SYS_CNTP_CTL_EL0 sys_reg(3, 3, 14, 2, 1)
#define SYS_CNTP_CVAL_EL0 sys_reg(3, 3, 14, 2, 2)

#define __emit_inst(x)			".inst " __stringify((x)) "\n\t"

asm(
"	.irp	num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30\n"
"	.equ	.L__reg_num_x\\num, \\num\n"
"	.endr\n"
"	.equ	.L__reg_num_xzr, 31\n"
"\n"
"	.macro	mrs_s, rt, sreg\n"
	__emit_inst(0xd5200000|(\\sreg)|(.L__reg_num_\\rt))
"	.endm\n"
"\n"
"	.macro	msr_s, sreg, rt\n"
	__emit_inst(0xd5000000|(\\sreg)|(.L__reg_num_\\rt))
"	.endm\n"
);

/*
 * Unlike read_cpuid, calls to read_sysreg are never expected to be
 * optimized away or replaced with synthetic values.
 */
#define read_sysreg(r) ({					\
	uint64_t __val;						\
	asm volatile("mrs %0, " __stringify(r) : "=r" (__val));	\
	__val;							\
})

/*
 * The "Z" constraint normally means a zero immediate, but when combined with
 * the "%x0" template means XZR.
 */
#define write_sysreg(v, r) do {					\
	uint64_t __val = (uint64_t)(v);					\
	asm volatile("msr " __stringify(r) ", %x0"		\
		     : : "rZ" (__val));				\
} while (0)

/*
 * For registers without architectural names, or simply unsupported by
 * GAS.
 */
#define read_sysreg_s(r) ({						\
	uint64_t __val;							\
	asm volatile("mrs_s %0, " __stringify(r) : "=r" (__val));	\
	__val;								\
})

#define write_sysreg_s(v, r) do {					\
	uint64_t __val = (uint64_t)(v);						\
	asm volatile("msr_s " __stringify(r) ", %x0" : : "rZ" (__val));	\
} while (0)

#endif /* __ASM_SYSREG_H */
