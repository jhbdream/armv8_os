#ifndef __LINK_AGE_H
#define __LINK_AGE_H

#include <compiler_attribute.h>
#include <asm/linkage.h>

#define __page_aligned_data                                                    \
	__section(".data..page_aligned") __aligned(PAGE_SIZE)
#define __page_aligned_bss __section(".bss..page_aligned") __aligned(PAGE_SIZE)

/* Some toolchains use other characters (e.g. '`') to mark new line in macro */
#ifndef ASM_NL
#define ASM_NL ;
#endif

#ifdef __ASSEMBLY__

#ifndef __ALIGN
#define __ALIGN .align 4, 0x90
#define __ALIGN_STR ".align 4,0x90"
#endif

#ifndef LINKER_SCRIPT
#define ALIGN __ALIGN
#define ALIGN_STR __ALIGN_STR

#ifndef GLOBAL
#define GLOBAL(name) .globl name ASM_NL name:
#endif

#ifndef ENTRY
#define ENTRY(name) .globl name ASM_NL ALIGN ASM_NL name:
#endif
#endif /* LINKER_SCRIPT */

#ifndef WEAK
#define WEAK(name) .weak name ASM_NL ALIGN ASM_NL name:
#endif

#ifndef END
#define END(name) .size name, .- name
#endif

#endif
#endif
