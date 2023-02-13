#ifndef __RWONCE_H__
#define __RWONCE_H__

#ifndef __READ_ONCE
#define __READ_ONCE(x) (*(const volatile typeof(x) *)&(x))
#endif

#define READ_ONCE(x) ({ __READ_ONCE(x); })

#define __WRITE_ONCE(x, val)                                                   \
	do {                                                                   \
		*(volatile typeof(x) *)&(x) = (val);                           \
	} while (0)

#define WRITE_ONCE(x, val)                                                     \
	do {                                                                   \
		__WRITE_ONCE(x, val);                                          \
	} while (0)

#endif
