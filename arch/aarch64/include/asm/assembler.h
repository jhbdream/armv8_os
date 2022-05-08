/**
 * @file assembler.h
 * @author jihongbin (longma@orbbec.com)
 * @brief
 * @version 0.1
 * @date 2022-05-07
 * @copyright Copyright (c) {2015-2021} Orbbec
 *
 */

#ifndef __ASSEMBLY__
#error "Only include this from assembly code"
#endif

#ifndef __ASM_ASSEMBLER_H
#define __ASM_ASSEMBLER_H

    /*
    * mov_q - move an immediate constant into a 64-bit register using
    *         between 2 and 4 movz/movk instructions (depending on the
    *         magnitude and sign of the operand)
    */
    .macro  mov_q, reg, val
    .if (((\val) >> 31) == 0 || ((\val) >> 31) == 0x1ffffffff)
    movz    \reg, :abs_g1_s:\val
    .else
    .if (((\val) >> 47) == 0 || ((\val) >> 47) == 0x1ffff)
    movz    \reg, :abs_g2_s:\val
    .else
    movz    \reg, :abs_g3:\val
    movk    \reg, :abs_g2_nc:\val
    .endif
    movk    \reg, :abs_g1_nc:\val
    .endif
    movk    \reg, :abs_g0_nc:\val
    .endm

    /*
     * in the process. This is called when setting the MMU on.
     */
    .macro set_sctlr, sreg, reg
       msr \sreg, \reg
       isb
       /*
        * Invalidate the local I-cache so that any instructions fetched
        * speculatively from the PoC are discarded, since they may have
        * been dynamically patched at the PoU.
        */
       ic  iallu
       dsb nsh
       isb
    .endm

    .macro set_sctlr_el1, reg
        set_sctlr sctlr_el1, \reg
    .endm

    .macro set_sctlr_el2, reg
        set_sctlr sctlr_el2, \reg
    .endm

#endif
