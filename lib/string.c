#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;
	while((*dest++ = *src++) != '\0')
	{

	}

	return tmp;
}

char *strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;
	while (count)
	{
		if((*tmp = *src) != 0)
		{
			src++;
		}

		tmp++;
		count--;
	}

	return dest;
}

int strcmp(const char *cs, const char *ct)
{
	unsigned char c1, c2;
	while (1)
	{
		c1 = *cs++;
		c2 = *ct++;

		if(c1 != c2)
		{
			return c1 < c2 ? 1 : -1;
		}

		if(!c1)
		{
			break;
		}
	}
	return 0;
}

int strncmp(const char *cs, const char *ct, size_t count)
{
	unsigned char c1, c2;
	while (count)
	{
		c1 = *cs++;
		c2 = *ct++;

		if(c1 != c2)
		{
			return c1 < c2 ? 1 : -1;
		}

		if(!c1)
		{
			break;
		}

		count--;
	}
	return 0;
}

size_t strlen(const char *s)
{
	const char *sc;

	for(sc = s; *sc != '\0'; ++sc)
		/* do nothing*/;

	return sc - s;
}

size_t strnlen(const char *s, size_t count)
{
	const char *sc;
	for(sc = s; count-- && *sc != '\0'; ++sc)
		/* do nothing*/;

	return sc - s;
}

char *strstr(const char *s1, const char *s2)
{
	size_t l1, l2;
	l2 = strlen(s2);
	if(!l2)
		return (char *)s1;

	l1 = strlen(s1);
	while (l1 >= l2)
	{
		l1--;
		if(!memcmp(s1, s2, l2))
		{
			return (char *)s1;
		}
		s1++;
	}
	return NULL;
}

char *strnsrt(const char *s1, const char *s2, size_t len)
{
	size_t l2;

	l2 = strlen(s2);
	if(!l2)
	{
		return (char *)s1;
	}

	while (len >= l2)
	{
		len--;
		if(!memcmp(s1, s2, l2))
		{
			return (char *)s1;
		}
		s1++;
	}

	return NULL;
}

int memcmp(const void *cs, const void *ct, size_t count)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for(su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, --count)
	{
		if((res = *su1 - *su2) != 0)
		{
			break;
		}
	}

	return res;
}

void *memchr(const void *s, int c, size_t n)
{
	const unsigned char *p = s;

	while (n-- != 0)
	{
		if((unsigned char)c == *p++)
		{
			return (void *)(p - 1);
		}
	}
	return NULL;
}

void *memcpy(void *dest, const void *src, size_t count)
{
	char *tmp = dest;
	const char *s = src;

	while (count--)
	{
		*tmp++ = *s++;
	}

	return dest;
}

void *memmove(void *dest, const void *src, size_t count)
{
	char *tmp;
	const char *s;

	if(dest <= src)
	{
		tmp = dest;
		s = src;
		while (count--)
		{
			*tmp++ = *s++;
		}
	}
	else
	{
		tmp = dest;
		tmp += count;
		s = src;
		s += count;
		while (count--)
		{
			*--tmp = *--s;
		}
	}
	return dest;
}

void *memset(void *s, int c, size_t count)
{
	char *xs = s;

	while (count--)
	{
		*xs++ = c;
	}
	return s;
}

unsigned long strtoul(const char *cp, char **endp, unsigned int base)
{
	unsigned long result = 0;

	if(!base)
	{
		base = 10;
	}
	
	if(base == 16 && cp[0] == '0' && tolower(cp[1]) == 'x')
	{
		cp += 2;
	}
	
	while(isxdigit(*cp))
	{
		unsigned int value;
		value = isdigit(*cp) ? *cp - '0' : tolower(*cp) - 'a' + 10;
		if(value > base)
			break;
		
		result = result * base + value;
		cp++;
	}
	
	if(endp)
		*endp = (char *)cp;

	return result;
}

char *strchr(const char *src, int ch)
{
	for (; *src != (char)ch; ++src)
		if (*src == '\0')
			return NULL;
	return (char *)src;
}

/**
 * strrchr - Find the last occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    do {
        if (*s == (char)c)
            last = s;
    } while (*s++);
    return (char *)last;
}

