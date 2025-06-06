#include <stddef.h>

#include <io.h>
#include <type.h>
#include <driver/console.h>

#include <asm/memory.h>

#define QEMU_UART_DR       ((void *)(DEVICE_START + 0x09000000 + UART01x_DR))
#define QEMU_UART_FR       ((void *)(DEVICE_START + 0x09000000 + UART01x_FR))

#define UART01x_DR         0x00 /* Data read or written from the interface. */
#define UART01x_RSR        0x04 /* Receive status register (Read). */
#define UART01x_ECR        0x04 /* Error clear register (Write). */
#define UART010_LCRH       0x08 /* Line control register, high byte. */
#define ST_UART011_DMAWM   0x08 /* DMA watermark configure register. */
#define UART010_LCRM       0x0C /* Line control register, middle byte. */
#define ST_UART011_TIMEOUT 0x0C /* Timeout period register. */
#define UART010_LCRL       0x10 /* Line control register, low byte. */
#define UART010_CR         0x14 /* Control register. */
#define UART01x_FR         0x18 /* Flag register (Read only). */
#define UART010_IIR        0x1C /* Interrupt identification register (Read). */
#define UART010_ICR        0x1C /* Interrupt clear register (Write). */
#define ST_UART011_LCRH_RX 0x1C /* Rx line control register. */
#define UART01x_ILPR       0x20 /* IrDA low power counter register. */
#define UART011_IBRD       0x24 /* Integer baud rate divisor register. */
#define UART011_FBRD       0x28 /* Fractional baud rate divisor register. */
#define UART011_LCRH       0x2c /* Line control register. */
#define ST_UART011_LCRH_TX 0x2c /* Tx Line control register. */
#define UART011_CR         0x30 /* Control register. */
#define UART011_IFLS       0x34 /* Interrupt fifo level select. */
#define UART011_IMSC       0x38 /* Interrupt mask. */
#define UART011_RIS        0x3c /* Raw interrupt status. */
#define UART011_MIS        0x40 /* Masked interrupt status. */
#define UART011_ICR        0x44 /* Interrupt clear register. */

/**
 * @brief
 *
 * @param ch
 * @return int
 */
int uart_getchar(uint8_t *ch)
{
    if (readb(QEMU_UART_FR) & 0x10) {
        *ch = (uint8_t)readb(QEMU_UART_DR);
        return 0;
    }

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

    for (i = 0; i < n; i++, s++) {

        if (*s == '\n') {
            uart_putchar('\r');
        }

        uart_putchar(*s);
    }
}

struct console __console_qemu SECTION_CONSOLE = {
    .name  = "qemu",
    .arg   = NULL,
    .init  = NULL,
    .write = qemu_console_write,
    .read  = NULL,
};
