#include <ee_stdarg.h>
#include <ee_stdint.h>
#include <ee_string.h>
#include <ee_stdio.h>
#include <limits.h>
#include <type.h>
#include <ctype.h>
#include <div64.h>

const char hex_asc[] = "0123456789abcdef";
const char hex_asc_upper[] = "0123456789ABCDEF";

#define SIGN 1	   /* unsigned/signed, must be 1 */
#define LEFT 2	   /* left justified */
#define PLUS 4	   /* show plus */
#define SPACE 8	   /* space if plus */
#define ZEROPAD 16 /* pad with zero, must be 16 == '0' - ' ' */
#define SMALL 32   /* use lowercase in hex (must be 32 == 0x20) */
#define SPECIAL 64 /* prefix hex with "0x", octal with "0" */

enum format_type
{
	FORMAT_TYPE_NONE, /* Just a string part */
	FORMAT_TYPE_WIDTH,
	FORMAT_TYPE_PRECISION,
	FORMAT_TYPE_CHAR,
	FORMAT_TYPE_STR,
	FORMAT_TYPE_PTR,
	FORMAT_TYPE_PERCENT_CHAR,
	FORMAT_TYPE_INVALID,
	FORMAT_TYPE_LONG_LONG,
	FORMAT_TYPE_ULONG,
	FORMAT_TYPE_LONG,
	FORMAT_TYPE_UBYTE,
	FORMAT_TYPE_BYTE,
	FORMAT_TYPE_USHORT,
	FORMAT_TYPE_SHORT,
	FORMAT_TYPE_UINT,
	FORMAT_TYPE_INT,
	FORMAT_TYPE_SIZE_T,
};

struct printf_spec
{
	unsigned int type : 8;		 /* format_type enum */
	signed int field_width : 24; /* width of output field */
	unsigned int flags : 8;		 /* flags to number() */
	unsigned int base : 8;		 /* number base, 8, 10 or 16 only */
	signed int precision : 16;	 /* # of digits/chars */
};

static const u16 decpair[100] = {
#define _(x) (u16)(((x % 10) | ((x / 10) << 8)) + 0x3030)
	_(0),
	_(1),
	_(2),
	_(3),
	_(4),
	_(5),
	_(6),
	_(7),
	_(8),
	_(9),
	_(10),
	_(11),
	_(12),
	_(13),
	_(14),
	_(15),
	_(16),
	_(17),
	_(18),
	_(19),
	_(20),
	_(21),
	_(22),
	_(23),
	_(24),
	_(25),
	_(26),
	_(27),
	_(28),
	_(29),
	_(30),
	_(31),
	_(32),
	_(33),
	_(34),
	_(35),
	_(36),
	_(37),
	_(38),
	_(39),
	_(40),
	_(41),
	_(42),
	_(43),
	_(44),
	_(45),
	_(46),
	_(47),
	_(48),
	_(49),
	_(50),
	_(51),
	_(52),
	_(53),
	_(54),
	_(55),
	_(56),
	_(57),
	_(58),
	_(59),
	_(60),
	_(61),
	_(62),
	_(63),
	_(64),
	_(65),
	_(66),
	_(67),
	_(68),
	_(69),
	_(70),
	_(71),
	_(72),
	_(73),
	_(74),
	_(75),
	_(76),
	_(77),
	_(78),
	_(79),
	_(80),
	_(81),
	_(82),
	_(83),
	_(84),
	_(85),
	_(86),
	_(87),
	_(88),
	_(89),
	_(90),
	_(91),
	_(92),
	_(93),
	_(94),
	_(95),
	_(96),
	_(97),
	_(98),
	_(99),
#undef _
};

static char *put_dec_trunc8(char *buf, unsigned r)
{
	unsigned q;

	/* 1 <= r < 10^8 */
	if (r < 100)
		goto out_r;

	/* 100 <= r < 10^8 */
	q = (r * (u64)0x28f5c29) >> 32;
	*((u16 *)buf) = decpair[r - 100 * q];
	buf += 2;

	/* 1 <= q < 10^6 */
	if (q < 100)
		goto out_q;

	/*  100 <= q < 10^6 */
	r = (q * (u64)0x28f5c29) >> 32;
	*((u16 *)buf) = decpair[q - 100 * r];
	buf += 2;

	/* 1 <= r < 10^4 */
	if (r < 100)
		goto out_r;

	/* 100 <= r < 10^4 */
	q = (r * 0x147b) >> 19;
	*((u16 *)buf) = decpair[r - 100 * q];
	buf += 2;
out_q:
	/* 1 <= q < 100 */
	r = q;
out_r:
	/* 1 <= r < 100 */
	*((u16 *)buf) = decpair[r];
	buf += r < 10 ? 1 : 2;
	return buf;
}

static char *put_dec_full8(char *buf, unsigned r)
{
	unsigned q;

	/* 0 <= r < 10^8 */
	q = (r * (u64)0x28f5c29) >> 32;
	*((u16 *)buf) = decpair[r - 100 * q];
	buf += 2;

	/* 0 <= q < 10^6 */
	r = (q * (u64)0x28f5c29) >> 32;
	*((u16 *)buf) = decpair[q - 100 * r];
	buf += 2;

	/* 0 <= r < 10^4 */
	q = (r * 0x147b) >> 19;
	*((u16 *)buf) = decpair[r - 100 * q];
	buf += 2;

	/* 0 <= q < 100 */
	*((u16 *)buf) = decpair[q];
	buf += 2;
	return buf;
}

static char *put_dec(char *buf, unsigned long long n)
{
	if (n >= 100 * 1000 * 1000)
		buf = put_dec_full8(buf, do_div(n, 100 * 1000 * 1000));
	/* 1 <= n <= 1.6e11 */
	if (n >= 100 * 1000 * 1000)
		buf = put_dec_full8(buf, do_div(n, 100 * 1000 * 1000));
	/* 1 <= n < 1e8 */
	return put_dec_trunc8(buf, n);
}

static char *number(char *buf, char *end, unsigned long long num,
					struct printf_spec spec)
{
	char tmp[3 * sizeof(num)];
	char sign;
	char locase;
	int need_pfx = ((spec.flags & SPECIAL) && spec.base != 10);
	int i;
	bool is_zero = num == 0LL;
	int field_width = spec.field_width;
	int precision = spec.precision;

	/* locase = 0 or 0x20. ORing digits or letters with 'locase'
	 * produces same digits or (maybe lowercased) letters */
	locase = (spec.flags & SMALL);
	if (spec.flags & LEFT)
		spec.flags &= ~ZEROPAD;
	sign = 0;

	if (spec.flags & SIGN)
	{
		if ((signed long long)num < 0)
		{
			sign = '-';
			num = -(signed long long)num;
			field_width--;
		}
		else if (spec.flags & PLUS)
		{
			sign = '+';
			field_width--;
		}
		else if (spec.flags & SPACE)
		{
			sign = ' ';
			field_width--;
		}
	}
	if (need_pfx)
	{
		if (spec.base == 16)
			field_width -= 2;
		else if (!is_zero)
			field_width--;
	}

	i = 0;
	if (num < spec.base)
	{
		tmp[i++] = hex_asc_upper[num] | locase;
	}
	else if (spec.base != 10)
	{
		int mask = spec.base - 1;
		int shift = 3;

		if (spec.base == 16)
		{
			shift = 4;
		}

		do
		{
			tmp[i++] = (hex_asc_upper[((unsigned char)num) & mask] | locase);
			num >>= shift;
		} while (num);
	}
	else
	{
		i = (put_dec(tmp, num) - tmp);
	}

	if (i > precision)
	{
		precision = i;
	}
	field_width -= precision;
	if (!(spec.flags & (ZEROPAD | LEFT)))
	{
		while (--field_width > 0)
		{
			if (buf < end)
			{
				*buf = ' ';
			}
			++buf;
		}
	}

	/* sign */
	if (sign)
	{
		if (buf < end)
			*buf = sign;
		++buf;
	}

	/* "0x" / "0" prefix %#x */
	if (need_pfx)
	{
		if (spec.base == 16 || !is_zero)
		{
			if (buf < end)
				*buf = '0';
			++buf;
		}
		if (spec.base == 16)
		{
			if (buf < end)
				*buf = ('X' | locase);
			++buf;
		}
	}

	/* zero or space padding */
	if (!(spec.flags & LEFT))
	{
		char c = ' ' + (spec.flags & ZEROPAD);

		while (--field_width >= 0)
		{
			if (buf < end)
				*buf = c;
			++buf;
		}
	}

	/* hmm even more zero padding? */
	while (i <= --precision)
	{
		if (buf < end)
			*buf = '0';
		++buf;
	}

	/* actual digits of result */
	while (--i >= 0)
	{
		if (buf < end)
			*buf = tmp[i];
		++buf;
	}

	/* trailing space padding */
	while (--field_width >= 0)
	{
		if (buf < end)
			*buf = ' ';
		++buf;
	}

	return buf;
}

static char *pointer_string(char *buf, char *end,
							const void *ptr,
							struct printf_spec spec)
{
	spec.base = 16;
	spec.flags |= SMALL;
	if (spec.field_width == -1)
	{
		spec.field_width = 2 * sizeof(ptr);
		spec.flags |= ZEROPAD;
	}

	return number(buf, end, (unsigned long int)ptr, spec);
}

static int skip_atoi(const char **s)
{
	int i = 0;

	do
	{
		i = i * 10 + *((*s)++) - '0';
	} while (isdigit(**s));

	return i;
}

static void set_field_width(struct printf_spec *spec, int width)
{
	spec->field_width = width;
}

static void set_precision(struct printf_spec *spec, int prec)
{
	spec->precision = prec;
}

static void move_right(char *buf, char *end, unsigned len, unsigned spaces)
{
	size_t size;
	if (buf >= end)
	{
		return;
	}

	size = end - buf;
	if (size <= spaces)
	{
		memset(buf, ' ', size);
		return;
	}

	if (len)
	{
		if (len > size - spaces)
		{
			len = size - spaces;
		}
		memmove(buf + spaces, buf, len);
	}
	memset(buf, ' ', spaces);
}

static char *widen_string(char *buf, int n, char *end, struct printf_spec spec)
{
	unsigned spaces;

	if (n > spec.field_width)
	{
		return buf;
	}

	spaces = spec.field_width - n;
	if (!(spec.flags & LEFT))
	{
		move_right(buf - n, end, n, spaces);
		return buf + spaces;
	}

	while (spaces--)
	{
		if (buf < end)
		{
			*buf = ' ';
		}
		++buf;
	}
	return buf;
}

static char *string(char *buf, char *end, const char *s, struct printf_spec spec)
{
	int len = 0;
	int lim = spec.precision;

	while (lim--)
	{
		char c = *s++;
		if (!c)
		{
			break;
		}
		if (buf < end)
		{
			*buf = c;
		}

		++buf;
		++len;
	}
	return widen_string(buf, len, end, spec);
}

int format_decode(const char *fmt, struct printf_spec *spec)
{
	const char *start = fmt;
	char qualifier;

	if (spec->type == FORMAT_TYPE_WIDTH)
	{
		if (spec->field_width < 0)
		{
			spec->field_width = -spec->field_width;
			spec->flags |= LEFT;
		}
		spec->type = FORMAT_TYPE_NONE;
		goto precision;
	}

	if (spec->type == FORMAT_TYPE_PRECISION)
	{
		if (spec->precision < 0)
		{
			spec->precision = 0;
		}
		spec->type = FORMAT_TYPE_NONE;
		goto qualifier;
	}

	spec->type = FORMAT_TYPE_NONE;
	for (; *fmt; ++fmt)
	{
		if (*fmt == '%')
		{
			break;
		}
	}
	if (fmt != start || !*fmt)
	{
		return fmt - start;
	}

	spec->flags = 0;
	while (1)
	{
		bool found = true;
		++fmt;

		switch (*fmt)
		{
		case '-':
			spec->flags |= LEFT;
			break;
		case '+':
			spec->flags |= PLUS;
			break;
		case ' ':
			spec->flags |= SPACE;
			break;
		case '#':
			spec->flags |= SPECIAL;
			break;
		case '0':
			spec->flags |= ZEROPAD;
			break;

		default:
			found = false;
			break;
		}

		if (!found)
			break;
	}

	spec->field_width = -1;
	if (isdigit(*fmt))
	{
		spec->field_width = skip_atoi(&fmt);
	}
	else if (*fmt == '*') //*号表示宽度由参数指出
	{
		spec->type = FORMAT_TYPE_WIDTH;
		return ++fmt - start;
	}

//精度 %.3f
precision:
	spec->precision = -1;
	if (*fmt == '.')
	{
		++fmt;
		if (isdigit(*fmt))
		{
			spec->precision = skip_atoi(&fmt);
			if (spec->precision < 0)
			{
				spec->precision = 0;
			}
		}
		else if (*fmt == '*')
		{
			spec->type = FORMAT_TYPE_PRECISION;
			return ++fmt - start;
		}
	}

qualifier:
	qualifier = 0;
	// hh = H ll = L
	if (*fmt == 'h' || _tolower(*fmt) == 'l' || *fmt == 'z' || *fmt == 't')
	{
		qualifier = *fmt++;
		if (qualifier == *fmt)
		{
			if (qualifier == 'l')
			{
				qualifier = 'L';
				++fmt;
			}
			else if (qualifier == 'h')
			{
				qualifier = 'H';
				++fmt;
			}
		}
	}

	spec->base = 10;
	switch (*fmt)
	{
	case 'c':
		spec->type = FORMAT_TYPE_CHAR;
		return ++fmt - start;
	case 's':
		spec->type = FORMAT_TYPE_STR;
		return ++fmt - start;
	case 'p':
		spec->type = FORMAT_TYPE_PTR;
		return ++fmt - start;
	case '%':
		spec->type = FORMAT_TYPE_PERCENT_CHAR;
		return ++fmt - start;
	case 'o':
		spec->base = 8;
		break;

	case 'x':
		spec->flags |= SMALL;
	case 'X':
		spec->base = 16;
		break;

	case 'd':
	case 'i':
		spec->flags |= SIGN;
		break;

	case 'u':
		break;

	case 'n':
	default:
		spec->type = FORMAT_TYPE_INVALID;
		return fmt - start;
	}

	if (qualifier == 'L')
	{
		spec->type = FORMAT_TYPE_LONG_LONG;
	}
	else if (qualifier == 'l')
	{
		spec->type = FORMAT_TYPE_ULONG + (spec->flags & SIGN);
	}
	else if (qualifier == 'z')
	{
		spec->type = FORMAT_TYPE_SIZE_T;
	}
	else if (qualifier == 'H')
	{
		spec->type = FORMAT_TYPE_UBYTE + (spec->flags & SIGN);
	}
	else if (qualifier == 'h')
	{
		spec->type = FORMAT_TYPE_USHORT + (spec->flags & SIGN);
	}
	else
	{
		spec->type = FORMAT_TYPE_UINT + (spec->flags & SIGN);
	}

	return ++fmt - start;
}

/**
 * vsprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * The function returns the number of characters written
 * into @buf. Use vsnprintf() or vscnprintf() in order to avoid
 * buffer overflows.
 *
 * If you're not already dealing with a va_list consider using sprintf().
 *
 * See the vsnprintf() documentation for format string extensions over C99.
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
	return vsnprintf(buf, INT_MAX, fmt, args);
}

/**
 * vsnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	unsigned long long num;
	char *str, *end;
	struct printf_spec spec = {0};

	if (size > INT_MAX)
	{
		return 0;
	}

	str = buf;
	end = buf + size;

	if (end < buf)
	{
		end = ((void *)-1);
		size = end - buf;
	}

	while (*fmt)
	{
		const char *old_fmt = fmt;
		int read = format_decode(fmt, &spec);
		int copy;
		char c;
		fmt += read;
		switch (spec.type)
		{
		case FORMAT_TYPE_NONE:
			copy = read;
			if (str < end)
			{
				if (copy > end - str)
				{
					copy = end - str;
				}
				memcpy(str, old_fmt, copy);
			}
			str += read;
			break;

		case FORMAT_TYPE_WIDTH:
			set_field_width(&spec, va_arg(args, int));
			break;

		case FORMAT_TYPE_PRECISION:
			set_precision(&spec, va_arg(args, int));
			break;

		case FORMAT_TYPE_CHAR:

			if (!(spec.flags & LEFT))
			{
				while (--spec.field_width > 0)
				{
					if (str < end)
					{
						*str = ' ';
					}
					++str;
				}
			}

			c = (unsigned char)va_arg(args, int);
			if (str < end)
			{
				*str = c;
			}
			++str;

			while (--spec.field_width > 0)
			{
				if (str < end)
				{
					*str = ' ';
				}
				++str;
			}

			break;
		case FORMAT_TYPE_STR:
			str = string(str, end, va_arg(args, void *), spec);
			break;

		case FORMAT_TYPE_PTR:
			str = pointer_string(str, end, va_arg(args, void *), spec);
			while (isalnum(*fmt))
				fmt++;
			break;

		case FORMAT_TYPE_PERCENT_CHAR:
			if (str < end)
			{
				*str = '%';
			}
			++str;
			break;

		case FORMAT_TYPE_INVALID:
			goto out;

		default:
			switch (spec.type)
			{
			case FORMAT_TYPE_LONG_LONG:
				num = va_arg(args, long long);
				break;

			case FORMAT_TYPE_ULONG:
				num = va_arg(args, unsigned long);
				break;

			case FORMAT_TYPE_LONG:
				num = va_arg(args, long);
				break;

			case FORMAT_TYPE_SIZE_T:
				if (spec.flags & SIGN)
					num = va_arg(args, ssize_t);
				else
					num = va_arg(args, size_t);
				break;
			case FORMAT_TYPE_UBYTE:
				num = (unsigned char)va_arg(args, int);
				break;
			case FORMAT_TYPE_BYTE:
				num = (signed char)va_arg(args, int);
				break;
			case FORMAT_TYPE_USHORT:
				num = (unsigned short)va_arg(args, int);
				break;
			case FORMAT_TYPE_SHORT:
				num = (short)va_arg(args, int);
				break;
			case FORMAT_TYPE_INT:
				num = (int)va_arg(args, int);
				break;
			default:
				num = va_arg(args, unsigned int);
			}
			str = number(str, end, num, spec);
		}
	}

out:
	if (size > 0)
	{
		if (str < end)
		{
			*str = '\0';
		}
		else
		{
			end[-1] = '\0';
		}
	}

	return str - buf;
}

/**
 * sprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The function returns the number of characters written
 * into @buf. Use snprintf() or scnprintf() in order to avoid
 * buffer overflows.
 *
 * See the vsnprintf() documentation for format string extensions over C99.
 */
int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);

	return i;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}
