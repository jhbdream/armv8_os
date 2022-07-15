#include "driver/interrupt.h"
#include <io.h>
#include <config.h>
#include <driver/console.h>
#include <driver/uart.h>
#include <stddef.h>
#include <ee/irqflags.h>

#define UART_RBR_OFFSET		0	/* In:  Recieve Buffer Register */
#define UART_THR_OFFSET		0	/* Out: Transmitter Holding Register */
#define UART_DLL_OFFSET		0	/* Out: Divisor Latch Low */
#define UART_IER_OFFSET		1	/* I/O: Interrupt Enable Register */
#define UART_DLM_OFFSET		1	/* Out: Divisor Latch High */
#define UART_FCR_OFFSET		2	/* Out: FIFO Control Register */
#define UART_IIR_OFFSET		2	/* I/O: Interrupt Identification Register */
#define UART_LCR_OFFSET		3	/* Out: Line Control Register */
#define UART_MCR_OFFSET		4	/* Out: Modem Control Register */
#define UART_LSR_OFFSET		5	/* In:  Line Status Register */
#define UART_MSR_OFFSET		6	/* In:  Modem Status Register */
#define UART_SCR_OFFSET		7	/* I/O: Scratch Register */
#define UART_MDR1_OFFSET	8	/* I/O:  Mode Register */

#define UART_LSR_FIFOE		0x80	/* Fifo error */
#define UART_LSR_TEMT		0x40	/* Transmitter empty */
#define UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#define UART_LSR_BI		0x10	/* Break interrupt indicator */
#define UART_LSR_FE		0x08	/* Frame error indicator */
#define UART_LSR_PE		0x04	/* Parity error indicator */
#define UART_LSR_OE		0x02	/* Overrun error indicator */
#define UART_LSR_DR		0x01	/* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS	0x1E	/* BI, FE, PE, OE bits */

/* clang-format on */

static volatile char *uart8250_base;
static u32 uart8250_in_freq;
static u32 uart8250_baudrate;
static u32 uart8250_reg_width;
static u32 uart8250_reg_shift;

static u32 get_reg(u32 num)
{
	u32 offset = num << uart8250_reg_shift;

	if (uart8250_reg_width == 1)
		return readb(uart8250_base + offset);
	else if (uart8250_reg_width == 2)
		return readw(uart8250_base + offset);
	else
		return readl(uart8250_base + offset);
}

static void set_reg(u32 num, u32 val)
{
	u32 offset = num << uart8250_reg_shift;

	if (uart8250_reg_width == 1)
		writeb(val, uart8250_base + offset);
	else if (uart8250_reg_width == 2)
		writew(val, uart8250_base + offset);
	else
		writel(val, uart8250_base + offset);
}

static void uart8250_putc(char ch)
{
	while ((get_reg(UART_LSR_OFFSET) & UART_LSR_THRE) == 0)
		;

	set_reg(UART_THR_OFFSET, ch);
}

static int uart8250_getc(void)
{
	if (get_reg(UART_LSR_OFFSET) & UART_LSR_DR)
		return get_reg(UART_RBR_OFFSET);
	return -1;
}

#include <circ_buf.h>
#include <string.h>

#define UART_XMIT_SIZE 4096

#define uart_circ_empty(circ)		((circ)->head == (circ)->tail)
#define uart_circ_clear(circ)		((circ)->head = (circ)->tail = 0)

#define uart_circ_chars_pending(circ)	\
	(CIRC_CNT((circ)->head, (circ)->tail, UART_XMIT_SIZE))

#define uart_circ_chars_free(circ)	\
	(CIRC_SPACE((circ)->head, (circ)->tail, UART_XMIT_SIZE))

struct circ_buf _recv;
struct circ_buf *recv = &_recv;

struct circ_buf _xmit;
struct circ_buf *xmit = &_xmit;

void uart_port_init(void)
{
	static char xmit_buf[UART_XMIT_SIZE];
	xmit->buf = xmit_buf;
	uart_circ_clear(xmit);

	static char recv_buf[UART_XMIT_SIZE];
	recv->buf = recv_buf;
	uart_circ_clear(recv);
}

int uart_write(const unsigned char *buf, int count)
{
	struct circ_buf *circ = xmit;
	int c;

	c = CIRC_SPACE_TO_END(circ->head, circ->tail, UART_XMIT_SIZE);
	if (count < c)
		c = count;
	if (c <= 0)
		return -1;

	memcpy(circ->buf + circ->head, buf, c);
	circ->head = (circ->head + c) & (UART_XMIT_SIZE - 1);

	/* start write enable irq */
	writel(0x3, (void *)0x40807004);

	return 0;
}

int uart_read(char *buf)
{
	struct circ_buf *circ = recv;
	int c;
	int count;

	c = CIRC_CNT_TO_END(circ->head, circ->tail, UART_XMIT_SIZE);

	memcpy(buf, circ->buf + circ->tail, c);
	circ->tail = (circ->tail + c) & (UART_XMIT_SIZE - 1);
	return c;
}

void uart_interrupt(struct irq_desc *desc)
{
	u32 status = get_reg(UART_IIR_OFFSET);

	if(status == 0x01)
	{
		return;
	}
	else if(status == 0x06)
	{
		get_reg(UART_LSR_OFFSET);
	}
	else if(status == 0x04)
	{
		/* uart recv data */
		u32 data = get_reg(UART_RBR_OFFSET);
		struct circ_buf *circ = recv;
		int count = 1;
		int c = CIRC_SPACE_TO_END(circ->head, circ->tail, UART_XMIT_SIZE);
		if (count < c)
			c = count;
		if (c <= 0)
			return;

		memcpy(circ->buf + circ->head, &data, c);
		circ->head = (circ->head + c) & (UART_XMIT_SIZE - 1);
	}
	else if(status == 0x02)
	{
		if(uart_circ_empty(xmit))
		{
			set_reg(UART_IER_OFFSET, 0x01);
			return;
		}

		set_reg(UART_THR_OFFSET, xmit->buf[xmit->tail]);
        xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);

		if(uart_circ_empty(xmit))
		{
			set_reg(UART_IER_OFFSET, 0x01);
		}
	}
	else if(status == 0x07)
	{

	}
}

void uart_thr_test(void)
{
	set_reg(UART_IER_OFFSET, 0x03);
	set_reg(UART_THR_OFFSET, 'b');
}

int uart8250_init(unsigned long base, u32 in_freq, u32 baudrate, u32 reg_shift,
		  u32 reg_width, u32 reg_offset)
{
	u16 bdiv;

	uart8250_base      = (volatile char *)base + reg_offset;
	uart8250_reg_shift = reg_shift;
	uart8250_reg_width = reg_width;
	uart8250_in_freq   = in_freq;
	uart8250_baudrate  = baudrate;

	bdiv = (uart8250_in_freq + 8 * uart8250_baudrate) / (16 * uart8250_baudrate);

	/* Disable all interrupts */
	set_reg(UART_IER_OFFSET, 0x00);
	/* Enable DLAB */
	set_reg(UART_LCR_OFFSET, 0x80);

	if (bdiv) {
		/* Set divisor low byte */
		set_reg(UART_DLL_OFFSET, bdiv & 0xff);
		/* Set divisor high byte */
		set_reg(UART_DLM_OFFSET, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	set_reg(UART_LCR_OFFSET, 0x03);
	/* Clear line status */
	get_reg(UART_LSR_OFFSET);
	/* Read receive buffer */
	get_reg(UART_RBR_OFFSET);

#if 0
	/* Enable interrupt */
	set_reg(UART_IER_OFFSET, 0x01);

	request_irq(28, uart_interrupt, 0x00, "8250uart", NULL);

	static char buff[4096];
	uart_port_init();

	while(1)
	{
		local_irq_disable();

		int cnt;
		cnt = uart_read(buff);
		if(cnt > 0)
		{
			uart_write(buff, cnt);
		}

		local_irq_enable();
	}
#endif

	return 0;
}

static int _init(struct console *con)
{
	return uart8250_init(0x40807000, 12000000, 57600, 2, 4, 0);
}

static void _write(struct console *con, const char *s, unsigned n)
{
	unsigned int i;
	for(i = 0; i < n; i++, s++)
	{
		if(*s == '\n')
		{
			uart8250_putc('\r');
		}
		uart8250_putc(*s);
	}
}
CONSOLE_DECLARE(uart8250, NULL, _init, _write, NULL);
