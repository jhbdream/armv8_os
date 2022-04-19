#include <io.h>
#include <config.h>
#include <driver/console.h>
#include <driver/uart.h>
#include <stddef.h>

#ifdef CONFIG_AARCH64
#define QEMU_UART_DR ((void __iomem *)(0x09000000 + UART01x_DR))
#define QEMU_UART_FR ((void __iomem *)(0x09000000 + UART01x_FR))
#endif

#ifdef CONFIG_RISCV64
#define QEMU_UART_DR ((void __iomem *)(0x10000000 + 0x00))
#define QEMU_UART_FR ((void __iomem *)(0x10000000 + 0x05))
#endif

#ifdef CONFIG_RISCV32
#define QEMU_UART_DR ((void __iomem *)(0x10000000 + 0x00))
#define QEMU_UART_FR ((void __iomem *)(0x10000000 + 0x05))
#endif


/**
 * @brief
 *
 * @param ch
 * @return int
 */
int uart_getchar(uint8_t *ch)
{
    if(readb(QEMU_UART_FR) & 0x10)
		*ch = (uint8_t)readb(QEMU_UART_DR);

	return -1;
}

int uart_putchar(uint8_t ch)
{
	writeb(ch, QEMU_UART_DR);
    return 0;
}

static void qemu_console_write(struct console *con, const char *s, unsigned n)
{
	unsigned int i;
	for(i = 0; i < n; i++, s++)
	{
		if(*s == '\n')
		{
			uart_putchar('\r');
		}
		uart_putchar(*s);
	}
}

CONSOLE_DECLARE(qemu, 0, qemu_console_write, NULL);
