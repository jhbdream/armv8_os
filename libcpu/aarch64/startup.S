    .section StartUp, "ax"
    .balign 4

    .global _startup
    .type _startup, "function"

_startup:

	ldr	x4, =0x09000000		// UART base
	mov	w5, #0x10		    // IBRD
	str	w5, [x4, #0x24]
	mov	w5, #0xc300
	orr	w5, w5, #0x0001		// CR
	str	w5, [x4, #0x30]

    mov w0, 'a'
    ldr x1, =0x09000000
    str w0, [x1]

    b main

_exit:
    wfi
    b exit