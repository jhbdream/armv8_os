OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH("aarch64")
ENTRY(_startup)

STACK_SIZE = 0x100;

SECTIONS
{
    . = 0x40000000;

    .startup :
    {
        __code_start = .;
        KEEP(*(StartUp))
    }

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
    }

    .text :
    {
        *(.text*)
    }

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
    }

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
	}

    .eh_frame :
    {
        KEEP (*(.eh_frame))
    }

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    }

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    }

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array ))
        PROVIDE_HIDDEN (__init_array_end = .);
    }

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array ))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }

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
    }

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    }

    .jcr :
    {
        KEEP (*(.jcr))
    }

    .data :
    {
        __data_start = . ;
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    }

    .bss :
    {
        . = ALIGN(64);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(64);
        __bss_end__ = .;
    }

    .init_pg_dir (NOLOAD):
    {
        . = ALIGN(4096);
        __init_pgd = .;
        . = . + 0x1000;
    }

    .init_pud (NOLOAD):
    {
        . = ALIGN(4096);
        __init_pud = .;
        . = . + 0x1000;
    }

    .init_pmd (NOLOAD):
    {
        . = ALIGN(4096);
        __init_pmd = .;
        . = . + 0x1000;
    }

    .init_pte (NOLOAD):
    {
        . = ALIGN(4096);
        __init_pte = .;
        . = . + 0x1000;
    }

    .heap (NOLOAD):
    {
        . = ALIGN(64);
        __end__ = .;
        PROVIDE(end = .);
        . = . + 0x1000;
    }

    .el1_stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + STACK_SIZE;
        __el1_stack = .;
        __stack_top = .;
    }

    /*
     * The startup code uses the end of this region to calculate
     * the top of memory - don't place any RAM regions after it
     */
    __top_of_ram = .;
}
