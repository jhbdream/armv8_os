#include <driver/uart.h>
#include <io.h>
#include <config.h>

#if CONFIG_AARCH64
#define QEMU_UART_DR ((void __iomem *)(0x09000000 + UART01x_DR))
#define QEMU_UART_FR ((void __iomem *)(0x09000000 + UART01x_FR))
#elif CONFIG_RISCV64
#define QEMU_UART_DR ((void __iomem *)(0x10000000 + 0x00))
#define QEMU_UART_FR ((void __iomem *)(0x10000000 + 0x05))
#else
#error "please select arch"
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
	// rv 读取该寄存器会出异常
    while((readb(QEMU_UART_FR) & 0x20) == 0)
	{

	}

	writeb(ch, QEMU_UART_DR);
    return 0;
}
