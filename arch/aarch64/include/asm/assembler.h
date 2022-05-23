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

/*
 * Pseudo-ops for PC-relative adr/ldr/str <reg>, <symbol> where
 * <symbol> is within the range +/- 4 GB of the PC.
 */
    /*
     * @dst: destination register (64 bit wide)
     * @sym: name of the symbol
     */
    .macro  adr_l, dst, sym
    adrp    \dst, \sym
    add \dst, \dst, :lo12:\sym
    .endm


/*
  * @dst: destination register (32 or 64 bit wide)
  * @sym: name of the symbol
  * @tmp: optional 64-bit scratch register to be used if <dst> is a
  *       32-bit wide register, in which case it cannot be used to hold
  *       the address
  */
    .macro  ldr_l, dst, sym, tmp=
    .ifb    \tmp
    adrp    \dst, \sym
    ldr \dst, [\dst, :lo12:\sym]
    .else
    adrp    \tmp, \sym
    ldr \dst, [\tmp, :lo12:\sym]
    .endif
    .endm

/*
 * @src: source register (32 or 64 bit wide)
 * @sym: name of the symbol
 * @tmp: mandatory 64-bit scratch register to calculate the address
 *       while <src> needs to be preserved.
 */
    .macro  str_l, src, sym, tmp
    adrp    \tmp, \sym
    str \src, [\tmp, :lo12:\sym]
    .endm

#endif
