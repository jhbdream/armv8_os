#include <driver/uart.h>

__attribute__((weak)) int uart_putchar(uint8_t ch);
__attribute__((weak)) int uart_getchar(uint8_t *ch);

int _read(int file, char *ptr, int len)
{
	int i;
	uint8_t ch;
	int read_len = 0;

	for (i = 0; i < len; ++i) {
		if (uart_getchar(&ch) == 0) {
			ptr[read_len] = ch;
			read_len++;
		}
	}
	return read_len;
}

int _write(int file, char *ptr, int len)
{
	int i;
	int write_len = 0;
	for (i = 0; i < len; ++i) {
		if (ptr[i] == '\n') {
			while (uart_putchar('\r') < 0)
				;
		}

		if (uart_putchar(ptr[write_len]) == 0) {
			write_len++;
		}
	}

	return write_len;
}
