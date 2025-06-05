#include <asm/assembler.h>
#include <asm/memory.h>

/*
 * Macro to populate page table entries, these entries can be pointers to the next level
 * or last level entries pointing to physical memory.
 *
 *  tbl:    page table address
 *  rtbl:   pointer to page table or physical memory
 *  index:  start index to write
 *  eindex: end index to write - [index, eindex] written to
 *  flags:  flags for pagetable entry to or in
 *  inc:    increment to rtbl between each entry
 *  tmp1:   temporary variable
 *
 * Preserves:   tbl, eindex, flags, inc
 * Corrupts:    index, tmp1
 * Returns: rtbl
 */
    .macro populate_entries, tbl, rtbl, index, eindex, flags, inc, tmp1
.Lpe\@:
    mov \tmp1, \rtbl
    orr \tmp1, \tmp1, \flags    // tmp1 = table entry
    str \tmp1, [\tbl, \index, lsl #3]
    add \rtbl, \rtbl, \inc  // rtbl = pa next level
    add \index, \index, #1
    cmp \index, \eindex
    b.ls    .Lpe\@
    .endm

/*
 * Compute indices of table entries from virtual address range. If multiple entries
 * were needed in the previous page table level then the next page table level is assumed
 * to be composed of multiple pages. (This effectively scales the end index).
 *
 *  vstart: virtual address of start of range
 *  vend:   virtual address of end of range - we map [vstart, vend]
 *  shift:  shift used to transform virtual address into index
 *  ptrs:   number of entries in page table
 *  istart: index in table corresponding to vstart
 *  iend:   index in table corresponding to vend
 *  count:  On entry: how many extra entries were required in previous level, scales
 *            our end index.
 *      On exit: returns how many extra entries required for next page table level
 *
 * Preserves:   vstart, vend, shift, ptrs
 * Returns: istart, iend, count
 */
    .macro compute_indices, vstart, vend, shift, ptrs, istart, iend, count
    lsr \iend, \vend, \shift  //获取 end index
    mov \istart, \ptrs
    sub \istart, \istart, #1
    and \iend, \iend, \istart   // iend = (vend >> shift) & (ptrs - 1)
    mov \istart, \ptrs
    mul \istart, \istart, \count
    add \iend, \iend, \istart   // iend += count * ptrs
                    // our entries span multiple tables

    lsr \istart, \vstart, \shift
    mov \count, \ptrs
    sub \count, \count, #1
    and \istart, \istart, \count

    sub \count, \iend, \istart
    .endm

/*
* Map memory for specified virtual address range. Each level of page table needed supports
* multiple entries. If a level requires n entries the next page table level is assumed to be
* formed from n pages.
*
*  tbl:    location of page table
*  rtbl:   address to be used for first level page table entry (typically tbl + PAGE_SIZE)
*  vstart: virtual address of start of range
*  vend:   virtual address of end of range - we map [vstart, vend - 1]
*  flags:  flags to use to map last level entries
*  phys:   physical address corresponding to vstart - physical memory is contiguous
*  pgds:   the number of pgd entries
*
* Temporaries: istart, iend, tmp, count, sv - these need to be different registers
* Preserves:   vstart, flags
* Corrupts:    tbl, rtbl, vend, istart, iend, tmp, count, sv
*/
    .macro map_memory, tbl, rtbl, vstart, vend, flags, phys, pgds, istart, iend, tmp, count, sv
    sub \vend, \vend, #1
    add \rtbl, \tbl, #4096
    mov \sv, \rtbl
    mov \count, #0
    compute_indices \vstart, \vend, #39, \pgds, \istart, \iend, \count
    populate_entries \tbl, \rtbl, \istart, \iend, #3, #4096, \tmp
    mov \tbl, \sv
    mov \sv, \rtbl

    compute_indices \vstart, \vend, #30, #512, \istart, \iend, \count
    populate_entries \tbl, \rtbl, \istart, \iend, #3, #4096, \tmp
    mov \tbl, \sv
    mov \sv, \rtbl

    compute_indices \vstart, \vend, #21, #512, \istart, \iend, \count
    populate_entries \tbl, \rtbl, \istart, \iend, #3, #4096, \tmp
    mov \tbl, \sv

    compute_indices \vstart, \vend, #12, #512, \istart, \iend, \count
    bic \count, \phys, #4095
    populate_entries \tbl, \count, \istart, \iend, \flags, #4096, \tmp
    .endm

    .section startup, "ax"
    .balign 4

    .global _startup
    .type _startup, "function"

    /* qemu default boot from el1 */
_startup:
	/* only boot core0 */
	mrs	x0, mpidr_el1
	and	x0, x0, #3
	cmp	x0, #0
	bne	hang

__init_el1:
    mov   x0, 0x3c5
    msr spsr_el1, x0

    msr spsel, #1

    adr x8, __init_el1_done
    msr elr_el1, x8
    eret

__init_el1_done:

    /* Create the identity mapping. */
    adrp  x0, __idmap_pg_start
    adrp  x3, __idmap_text_start
    adrp  x6, __idmap_text_end

    mov   x4, #512
	mov   x7, #(3 << 0) | (1 << 10) | (3 << 8)

    map_memory x0, x1, x3, x6, x7, x3, x4, x10, x11, x12, x13, x14

    adrp    x0, __init_pg_dir_start   // x0 = 1级页表基地址
    mov_q   x5, KIMAGE_VADDR          // x5 = image 虚拟地址的起始地址
    mov     x4, #512                  // x4 =页表项的大小
    adrp    x3, __kimage_start        // x3 = image 物理地址起始地址
    adrp    x6, __kimage_end
    sub     x6, x6, x3
    add     x6, x6, x5                // x6 = image 虚拟地址的结束地址

    /*
    *  Map the kernel image
    *
    *  x0: page table address
    *  x1: next level page table 这里是连续存放page table，自动根据 x0 + PAGE_SIZE 计算
    *  x5: 映射的虚拟地址起始位置
    *  x6: 映射的虚拟地址结束位置
    *  x7: 最后一级页表的一些参数
    *  x3: 映射的物理地址 起始位置
    *  x4: pgd entry 数量
    *  x 10, x11, x12, x13, x14: 临时寄存器、无需初始化
    */
    map_memory x0, x1, x5, x6, x7, x3, x4, x10, x11, x12, x13, x14

    /*
     * Since the page tables have been populated with non-cacheable
     * accesses (MMU disabled), invalidate those tables again to
     * remove any speculatively loaded cache lines.
     */
    dmb sy

__cpu_setup:
    /* Enable FP/SIMD at EL1 libc printk use float reg, if not enable fp will exception */
    mov x1, #3 << 20
    msr cpacr_el1, x1           // Enable FP/ASIMD

    /* disable all exception */
    msr daifset, 0xf

__enable_mmu:
    tlbi    vmalle1             // Invalidate local TLB
    dsb nsh

    mov_q x1, 0x40044ffff
    msr mair_el1, x1

    mov_q x1, 0x70b5503510
    msr tcr_el1, x1

    adrp x1, __init_pg_dir_start
    adrp x2, __idmap_pg_start

    msr ttbr0_el1, x2           // load TTBR0
    msr ttbr1_el1, x1           // load TTBR1

    isb

    // 设置 sctlr 寄存器 打开mmu功能
    mov_q           x0, 0x200000034f4d91d
    set_sctlr_el1   x0


    // 用于计算 kimage offset
    adrp x0, __kimage_start

    // 获取虚拟地址 跳转到虚拟地址执行
    ldr x8,  =__mmu_on
    blr x8

__mmu_on:

    ldr x8, =vectors
    msr	vbar_el1, x8

bss_clear:
    // Zero the bss
    ldr x2, =__bss_start__ // Start of block
    ldr x3, =__bss_end__   // End of block

memset_loop:
    str wzr, [x2], #4
    cmp x2, x3
    b.lt memset_loop

    ldr x1, =__kimage_start
    sub x1, x1, x0
    str_l x1, kimage_voffset, x5

    /* 初始化栈指针 */
    adr x8, __stack_top
    mov sp, x8

    b .
    nop

    // 创建临时的镜像映射 此时的页表是使用链接脚本分配的空间
    // 正式的页表需要后续使用动态分配内存创建映射关系、然后在切换页表
    bl start_kernel

hang:
	wfi
	b	hang
