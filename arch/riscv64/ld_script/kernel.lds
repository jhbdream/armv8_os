OUTPUT_ARCH( "riscv" )
OUTPUT_FORMAT("elf64-littleriscv")
ENTRY(_startup)

STACK_SIZE = 0x10000;

SECTIONS
{
	/* qemu riscv64 entry point */
	. = 0x80000000;

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
		. = ALIGN(8);
		__bss_start = .;
		*(.bss .bss.*)
		. = ALIGN(8);
		__bss_stop = .;
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

	.stack (NOLOAD):
    {
        . = ALIGN(8);
        . = . + STACK_SIZE;
        . = ALIGN(8);
        __stack_top = .;
    } 
}
