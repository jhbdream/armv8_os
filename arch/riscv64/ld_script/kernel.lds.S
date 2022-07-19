OUTPUT_ARCH( "riscv" )
OUTPUT_FORMAT("elf64-littleriscv")
ENTRY(_startup)

STACK_SIZE = 0x10000;

#include <asm/memory.h>

SECTIONS
{
    . = KIMAGE_VADDR;
    PROVIDE(__kimage_start = .);

	.startup :
    {
        __code_start = .;
        KEEP(*(StartUp))
    }

	.text :
	{
		*(.text .text.*)
	}

	.data :
	{
		*(.data .data.*)

        . = ALIGN(8);
        _shell_command_start = .;
        KEEP (*(shellCommand))
        _shell_command_end = .;

		. = ALIGN(8);
		__console_device_begin = .;
		KEEP (*(__console));
		__console_device_end = .;
	}

	.bss :
	{
		. = ALIGN(8);
		__bss_start = .;
		*(.bss .bss.*)
		*(.sbss)
		*(.sbss.*)
		. = ALIGN(8);
		__bss_stop = .;
	}

	.stack (NOLOAD):
    {
        . = ALIGN(16);
        . = . + STACK_SIZE;
        . = ALIGN(16);
        __stack_top = .;
    }

    . = ALIGN(4096);
    PROVIDE(__kimage_end = .);
}
