#include <type.h>
#include <io.h>
#include <driver/console.h>
#include <driver/uart.h>
#include <stddef.h>

#include <asm/memory.h>

#define QEMU_UART_DR ((void *)(DEVICE_START + 0x09000000 + UART01x_DR))
#define QEMU_UART_FR ((void *)(DEVICE_START + 0x09000000 + UART01x_FR))

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
