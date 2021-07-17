#include <driver/uart.h>
#include <io.h>

#define QEMU_UART_DR (0x09000000)

uint8_t uart_getchar_polled(void)
{
    return readl(QEMU_UART_DR);
}

void uart_putc_polled(uint8_t c)
{
    writel(c, QEMU_UART_DR);
}