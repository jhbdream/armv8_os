#ifndef __LINK_AGE_H
#define __LINK_AGE_H

/* Some toolchains use other characters (e.g. '`') to mark new line in macro */
#ifndef ASM_NL
#define ASM_NL		 ;
#endif

#ifdef __ASSEMBLY__

#ifndef __ALIGN
#define __ALIGN		.align 4,0x90
#define __ALIGN_STR	".align 4,0x90"
#endif

#ifndef ENTRY
#define ENTRY(name) \
	.globl name ASM_NL \
	ALIGN ASM_NL \
	name:
#endif

#ifndef WEAK
#define WEAK(name)	   \
	.weak name ASM_NL   \
	ALIGN ASM_NL \
	name:
#endif

#ifndef END
#define END(name) \
	.size name, .-name
#endif

#endif
#endif
