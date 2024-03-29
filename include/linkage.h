#ifndef __LINKAGE_H
#define __LINKAGE_H

#include <compiler_attribute.h>

/* Some toolchains use other characters (e.g. '`') to mark new line in macro */
#ifndef ASM_NL
#define ASM_NL ;
#endif

#ifdef __ASSEMBLY__

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
