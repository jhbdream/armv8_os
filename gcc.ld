OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH("aarch64")
ENTRY(_startup)

STACK_SIZE = 0x100;

MEMORY
{
    ram  : org = 0x40000000, len = 32M
}

SECTIONS
{
    .startup :
    {
        __code_start = .;
        KEEP(*(StartUp))
    } > ram

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
    } > ram

    .text :
    {
        *(.text*)
    } > ram

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
    } > ram

    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)

        _shell_command_start = .;
        KEEP (*(shellCommand))
        _shell_command_end = .;
    } > ram

    .eh_frame :
    {
        KEEP (*(.eh_frame))
    } > ram

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ram

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > ram

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > ram

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array ))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > ram

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array ))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > ram

    .ctors :
    {
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
           the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } > ram

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } > ram

    .jcr :
    {
        KEEP (*(.jcr))
    } > ram

    .data :
    {
        __data_start = . ;
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    } > ram

    .bss :
    {
        . = ALIGN(64);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(64);
        __bss_end__ = .;
    } > ram

    .heap (NOLOAD):
    {
        . = ALIGN(64);
        __end__ = .;
        PROVIDE(end = .);
        . = . + 0x1000;
    } > ram

    .el1_stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + STACK_SIZE;
        __el1_stack = .;
        __stack_top = .;
    } > ram


    /*
     * The startup code uses the end of this region to calculate
     * the top of memory - don't place any RAM regions after it
     */
    __top_of_ram = .;
}
