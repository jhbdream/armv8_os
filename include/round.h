/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_ROUND_H
#define _LINUX_ROUND_H

#include <bits.h>

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define __round_mask(x, y) ((__typeof__(x))((y)-1))

/**
 * round_up - round up to next specified power of 2
 * @x: the value to round
 * @y: multiple to round up to (must be a power of 2)
 *
 * Rounds @x up to next multiple of @y (which must be a power of 2).
 * To perform arbitrary rounding up, use roundup() below.
 */
#define round_up(x, y) ((((x)-1) | __round_mask(x, y)) + 1)

/**
 * round_down - round down to next specified power of 2
 * @x: the value to round
 * @y: multiple to round down to (must be a power of 2)
 *
 * Rounds @x down to next multiple of @y (which must be a power of 2).
 * To perform arbitrary rounding down, use rounddown() below.
 */
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define DIV_ROUND_DOWN_ULL(ll, d)                                              \
	({                                                                     \
		unsigned long long _tmp = (ll);                                \
		do_div(_tmp, d);                                               \
		_tmp;                                                          \
	})

#define DIV_ROUND_UP_ULL(ll, d)                                                \
	DIV_ROUND_DOWN_ULL((unsigned long long)(ll) + (d)-1, (d))

#if BITS_PER_LONG == 32
#define DIV_ROUND_UP_SECTOR_T(ll, d) DIV_ROUND_UP_ULL(ll, d)
#else
#define DIV_ROUND_UP_SECTOR_T(ll, d) DIV_ROUND_UP(ll, d)
#endif

/**
 * roundup - round up to the next specified multiple
 * @x: the value to up
 * @y: multiple to round up to
 *
 * Rounds @x up to next multiple of @y. If @y will always be a power
 * of 2, consider using the faster round_up().
 */
#define roundup(x, y)                                                          \
	({                                                                     \
		typeof(y) __y = y;                                             \
		(((x) + (__y - 1)) / __y) * __y;                               \
	})
/**
 * rounddown - round down to next specified multiple
 * @x: the value to round
 * @y: multiple to round down to
 *
 * Rounds @x down to next multiple of @y. If @y will always be a power
 * of 2, consider using the faster round_down().
 */
#define rounddown(x, y)                                                        \
	({                                                                     \
		typeof(x) __x = (x);                                           \
		__x - (__x % (y));                                             \
	})

#endif