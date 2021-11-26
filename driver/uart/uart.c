#include <driver/uart.h>
#include <io.h>

#define QEMU_UART_DR ((void __iomem *)(0x09000000 + UART01x_DR))
#define QEMU_UART_FR ((void __iomem *)(0x09000000 + UART01x_FR))

/**
 * @brief
 *
 * @param ch
 * @return int
 */
int uart_getchar(uint8_t *ch)
{
    if(readl(QEMU_UART_FR) & 0x10)
        return -1;

    *ch = (uint8_t)readl(QEMU_UART_DR);
    return 0;
}

int uart_putchar(uint8_t ch)
{
    if(readl(QEMU_UART_FR) & 0x20)
        return -1;

    writel(ch, QEMU_UART_DR);
    return 0;
}