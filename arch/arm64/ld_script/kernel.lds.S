OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH("aarch64")
ENTRY(_startup)

PHDRS
{
    text    PT_LOAD    FLAGS(5);    /* R-X */
    data    PT_LOAD    FLAGS(6);    /* RW- */
}

SECTIONS
{
    . = 0xFFFF000000080000;
    PROVIDE(__kimage_start = .);

    .startup :
    {
        __idmap_text_start = .;
        KEEP(*(startup))
        __idmap_text_end = .;
    } :text

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
    } :text

    .text :
    {
        *(.text*)
    } :text

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
    } :text

    .rodata :
    {
        . = ALIGN(8);

        *(.rodata .rodata.* .gnu.linkonce.r.*)

        . = ALIGN(8);
        _shell_command_start = .;
        KEEP (*(shellCommand))
        _shell_command_end = .;

		. = ALIGN(8);
		__console_device_begin = .;
		KEEP (*(__console));
		__console_device_end = .;
	} :text

    .eh_frame :
    {
        KEEP (*(.eh_frame))
    } :text

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } :text

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } :text

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } :data

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array ))
        PROVIDE_HIDDEN (__init_array_end = .);
    } :data

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array ))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } :data

    .ctors :
    {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } :data

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } :data

    .jcr :
    {
        KEEP (*(.jcr))
    } :data

    .data :
    {
        __data_start = . ;
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    } :data

    .bss :
    {
        . = ALIGN(64);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(64);
        __bss_end__ = .;
    } :data

    .idmap_pg_dir (NOLOAD):
    {
        . = ALIGN(4096);
        __idmap_pg_start = .;
        . = . + (4096 * 4);
        __idmap_pg_end = .;
    } :data

    .init_pg_dir (NOLOAD):
    {
        . = ALIGN(4096);
        __init_pg_dir_start = .;
        . = . + (4096 * 3) + (4096 * 8);
        __init_pg_dir_end = .;
    } :data

    . = ALIGN(4096);
    swapper_pg_dir = .;

    .heap (NOLOAD):
    {
        . = ALIGN(64);
        __end__ = .;
        PROVIDE(end = .);
        . = . + 0x1000;
    } :data

    .el1_stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + 0x4000;
        . = ALIGN(64);
        __el1_stack = .;
        __stack_top = .;
    } :data

    /*
     * The startup code uses the end of this region to calculate
     * the top of memory - don't place any RAM regions after it
     */
    __top_of_ram = .;

    . = ALIGN(4096);
    PROVIDE(__kimage_end = .);
}
