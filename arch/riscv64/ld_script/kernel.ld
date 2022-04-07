OUTPUT_ARCH( "riscv" )
OUTPUT_FORMAT("elf64-littleriscv")
ENTRY(_startup)

SECTIONS
{
	/* qemu riscv64 entry point */
	. = 0x80200000;

	.startup :
    {
        __code_start = .;
        KEEP(*(StartUp))
    }

	.text : 
	{
		*(.text .text.*)
		. = ALIGN(4K);
	}

	.data :
	{
		*(.data .data.*)
	}

	.bss :
	{
		*(.bss .bss.*)
	}

	.rodata :
    {
        . = ALIGN(8);

        *(.rodata .rodata.* .gnu.linkonce.r.*)

        . = ALIGN(8);
        _shell_command_start = .;
        KEEP (*(shellCommand))
        _shell_command_end = .;
    } 
}
