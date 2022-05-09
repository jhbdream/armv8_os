#ifndef _EE_MINMAX_H
#define _EE_MINMAX_H

#define __typecheck(x, y) \
    (!!(sizeof((typeof(x) *)1 == (typeof(y) *)1)))

#define __cmp(x, y, op) ((x) op (y) ? (x) : (y))


/**
 * min - return minimum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define min(x, y)   __cmp(x, y, <)

#define max(x, y)   __cmp(x, y, >)

 #endif