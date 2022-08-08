# SPDX-License-Identifier: GPL-2.0
VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = -rc1
NAME = unstable

PHONY := _all
_all:

MAKEFLAGS += -rR --no-print-directory

ifeq ("$(origin V)", "command line")
  MBUILD_VERBOSE = $(V)
endif
ifndef MBUILD_VERBOSE
  MBUILD_VERBOSE = 0
endif

ifeq ($(MBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

ifeq ("$(origin O)", "command line")
	O_LEVEL = $(O)
endif
ifndef O_LEVEL
	O_LEVEL = 0
endif

ifeq ("$(origin GDB)", "command line")
	DEBUG_USE_GDB = $(GDB)
endif
ifndef DEBUG_USE_GDB
	DEBUG_USE_GDB = 0
endif

export quiet Q MBUILD_VERBOSE

srctree 	:= $(CURDIR)
objtree		:= $(CURDIR)
src			:= $(srctree)
obj			:= $(objtree)

VPATH		:= $(srctree)

export srctree objtree VPATH

version_h := include/config/version.h

clean-targets := %clean

MCONFIG_CONFIG  ?= .config
export MCONFIG_CONFIG

-include .config
include scripts/config.mk

KERNELVERSION = $(VERSION)$(if $(PATCHLEVEL),.$(PATCHLEVEL)$(if $(SUBLEVEL),.$(SUBLEVEL)))$(EXTRAVERSION)
export VERSION PATCHLEVEL SUBLEVEL KERNELVERSION

ifeq ($(CONFIG_ARCH_AARCH64), y)
	ARCH			= aarch64
	CROSS_COMPILE 	= aarch64-linux-gnu-
	BFD_NAME		= elf64-littleaarch64
else ifeq ($(CONFIG_ARCH_RISCV64), y)
	ARCH			= riscv64
	CROSS_COMPILE 	= riscv64-unknown-elf-
	BFD_NAME		= elf64-littleriscv
else ifeq ($(CONFIG_ARCH_RISCV32), y)
	ARCH			= riscv32
	CROSS_COMPILE 	= riscv32-unknown-elf-
endif

SRCARCH 	:= $(ARCH)


# Make variables (CC, etc...)
AS			= $(CROSS_COMPILE)as
LD			= $(CROSS_COMPILE)ld
CC			= $(CROSS_COMPILE)gcc
CPP			= $(CC) -E
AR			= $(CROSS_COMPILE)ar
NM			= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
LEX			= flex
YACC		= bison
AWK			= awk
PERL		= perl
PYTHON		= python
PYTHON2		= python2
PYTHON3		= python3
CHECK		= sparse
DTC			= dtc
CAT			= cat

EEOSINCLUDE    := \
		-I$(srctree)/arch/$(SRCARCH)/include \
		-I$(srctree)/include	\
		-I$(srctree)/include/kernel	\
		-I$(srctree)/include/config \
		-I$(srctree)/include/libc	\
		-I$(srctree)/lib/libfdt	\
		-I$(srctree)/include/ee

CSTD_FLAG := -std=gnu11
MBUILD_DEFINE := -D__KERNEL__ -D__DEBUG_USE_GDB__=$(DEBUG_USE_GDB)
NOSTDINC_FLAGS += -nostdinc

MBUILD_CFLAGS   := 	-Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
					-Wno-unused-function -Wno-unused-variable \
		   			-fno-strict-aliasing -fno-common -fshort-wchar \
					-fno-stack-protector \
		   			-Werror-implicit-function-declaration \
		   			-Wno-format-security -O$(O_LEVEL) \
		   			$(CSTD_FLAG) --static -nostdlib  -nostartfiles -fno-builtin	\
					-g $(EEOSINCLUDE) $(MBUILD_DEFINE) $(NOSTDINC_FLAGS)

MBUILD_AFLAGS   := -D__ASSEMBLY__
MBUILD_LDFLAGS := --no-undefined -static -nostdlib

export ARCH SRCARCH CROSS_COMPILE AS LD CC DTC
export CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE LEX YACC AWK GENKSYMS PERL PYTHON PYTHON2 PYTHON3

export NOSTDINC_FLAGS EEOSINCLUDE OBJCOPYFLAGS MBUILD_LDFLAGS
export MBUILD_CFLAGS
export MBUILD_AFLAGS

PHONY += all
_all: all

core-y		:= init/ kernel/ lib/
drivers-y	:= driver/
external-y	:=
libs-y		:=

# The arch Makefile can set ARCH_{CPP,A,C}FLAGS to override the default
# values of the respective MBUILD_* variables
ARCH_AFLAGS :=
ARCH_CFLAGS :=
ARCH_LDFLAGS :=
export ARCH_CFLAGS ARCH_AFLAGS ARCH_LDFLAGS

include arch/$(SRCARCH)/Makefile

MBUILD_IMAGE 		:= eeos.bin
MBUILD_IMAGE_DTB	:= eeos_dtb.bin
MBUILD_IMAGE_ELF 	:= eeos.elf
MBUILD_IMAGE_SYMBOLS	:= allsymbols.o

DEVICE_TREE ?= $(CONFIG_DEFAULT_DEVICE_TREE:"%"=%)
ifeq ($(DEVICE_TREE),)
DEVICE_TREE := unset
endif

ifneq ($(EXT_DTB),)
DTB := $(EXT_DTB)
else
DTB := arch/$(ARCH)/dts/$(DEVICE_TREE).dtb
endif       

DTB_SECTION := $(subst /,_,$(DTB))
DTB_SECTION := $(DTB_SECTION:%.dtb=%)
DTB_SECTION := $(addprefix _binary_, $(DTB_SECTION)) 
DTB_SECTION := $(addsuffix _dtb_start, $(DTB_SECTION)) 

all: include/config/config.h $(version_h) eeos

eeos-dirs	:= $(patsubst %/,%,$(filter %/, $(core-y) $(external-y) $(drivers-y) $(libs-y)))

eeos-alldirs	:= $(sort $(eeos-dirs) $(patsubst %/,%,$(filter %/, \
			$(core-) $(external-) $(drivers-) $(libs-))))

eeos-clean-dirs = $(eeos-dirs)
eeos-cleandirs = $(eeos-alldirs)

core-y		:= $(patsubst %/, %/built-in.o, $(core-y))
external-y	:= $(patsubst %/, %/built-in.o, $(external-y))
drivers-y	:= $(patsubst %/, %/built-in.o, $(drivers-y))
libs-y1		:= $(patsubst %/, %/lib.a, $(libs-y))
libs-y2		:= $(patsubst %/, %/built-in.o, $(filter-out %.o, $(libs-y)))

# Externally visible symbols (used by link-eeos.sh)
export MBUILD_EEOS_MAIN	:= $(core-y) $(libs-y2) $(drivers-y) $(external-y)
export MBUILD_EEOS_LIBS	:= $(libs-y1)
export MBUILD_LDS		:= $(objtree)/arch/$(SRCARCH)/ld_script/kernel.lds

eeos-deps := $(MBUILD_LDS) $(MBUILD_EEOS_MAIN) $(MBUILD_EEOS_LIBS)

CLEAN_DIRS	:=
clean: rm-dirs 	:= $(CLEAN_DIRS)
clean-dirs      := $(addprefix _clean_, . $(eeos-cleandirs))

eeos_LDFLAGS := $(MBUILD_LDFLAGS) $(ARCH_LDFLAGS)
eeos_LDFLAGS += -T$(MBUILD_LDS) -Map=$(srctree)/linkmap.txt

PHONY += $(clean-dirs) clean distclean
$(clean-dirs):
	$(Q) $(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

eeos: $(eeos-deps) scripts/generate_allsymbols.py
	$(Q) echo "  OBJCOPY $(DTB)"
	$(Q) $(OBJCOPY) -I binary -O $(BFD_NAME) $(DTB) .tmp.dtb.o
	$(Q) echo "  LD      .tmp.eeos.elf"
	$(Q) $(LD) $(eeos_LDFLAGS) -o .tmp.eeos.elf $(MBUILD_EEOS_MAIN) $(MBUILD_EEOS_LIBS) .tmp.dtb.o
	$(Q) echo "  NM      .tmp.eeos.symbols"
	$(Q) $(NM) -n .tmp.eeos.elf > .tmp.eeos.symbols
	$(Q) echo "  PYTHON  allsymbols.S"
	$(Q) python3 scripts/generate_allsymbols.py .tmp.eeos.symbols allsymbols.S
	$(Q) echo "  CC      $(MBUILD_IMAGE_SYMBOLS)"
	$(Q) $(CC) $(MBUILD_AFLAGS) $(MBUILD_CFLAGS) $(ARCH_CFLAGS) -c allsymbols.S -o $(MBUILD_IMAGE_SYMBOLS)
	$(Q) echo "  LD      $(MBUILD_IMAGE_ELF)"
	$(Q) $(LD) $(eeos_LDFLAGS) -o $(MBUILD_IMAGE_ELF) $(MBUILD_EEOS_MAIN) $(MBUILD_EEOS_LIBS) $(MBUILD_IMAGE_SYMBOLS) .tmp.dtb.o
	$(Q) echo "  OBJCOPY $(MBUILD_IMAGE)"
	$(Q) $(OBJCOPY) -O binary  $(MBUILD_IMAGE_ELF) $(MBUILD_IMAGE)
	$(Q) echo "  OBJDUMP eeos.dis"
	$(Q) $(OBJDUMP) $(MBUILD_IMAGE_ELF) -D > eeos.dis

# The actual objects are generated when descending,
# make sure no implicit rule kicks in
$(sort $(eeos-deps)): $(eeos-dirs)

# here goto each directory to generate built-in.o
PHONY += $(eeos-dirs)
$(eeos-dirs):
	$(Q)$(MAKE) $(build)=$@

define sed-y
    "/^->/{s:->#\(.*\):/* \1 */:; \
    s:^->\([^ ]*\) [\$$#]*\([^ ]*\) \(.*\):#define \1 \2 /* \3 */:; \
    s:->::; p;}"
endef

define filechk_version.h
	(echo \#define EEOS_VERSION_CODE $(shell                         \
	expr $(VERSION) \* 65536 + 0$(PATCHLEVEL) \* 256 + 0$(SUBLEVEL)) > $@; \
	echo '#define EEOS_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))' >> $@; \
	echo '#define EEOS_VERSION_STR "v$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION) $(NAME)"' >> $@;)
endef

define device_tree_config.h
	(echo '#define DTB_START_SYMBOL $(DTB_SECTION)' >> $@;)
endef


PHONY += scriptconfig iscriptconfig menuconfig guiconfig dumpvarsconfig

PYTHONCMD ?= python3
kpython := PYTHONPATH=$(srctree)/scripts/Kconfiglib:$$PYTHONPATH $(PYTHONCMD)
KCONFIG ?= $(srctree)/Kconfig

ifneq ($(filter scriptconfig,$(MAKECMDGOALS)),)
ifndef SCRIPT
$(error Use "make scriptconfig SCRIPT=<path to script> [SCRIPT_ARG=<argument>]")
endif
endif

$(version_h) : Makefile
	$(Q) mkdir -p include/config
	$(Q) $(call filechk_version.h)

PHONY += include/config/config.h
include/config/config.h: .config
	$(Q) mkdir -p include/config
	$(Q) $(kpython) $(srctree)/scripts/Kconfiglib/genconfig.py --header-path=include/config/config.h
	$(Q) $(call device_tree_config.h)

clean: $(clean-dirs)
	$(Q) echo "  CLEAN   all .o .*.d *.dtb built-in.o"
	$(Q) echo "  CLEAN   allsymbols.o allsymbols.S linkmap.txt eeos.s .tmp.eeos.elf .tmp.eeos.symbols eeos.bin eeos.elf eeos.dis .tmp.dtb.o"
	$(Q) rm -f allsymbols.o allsymbols.S linkmap.txt eeos.s .tmp.eeos.elf .tmp.eeos.symbols eeos.bin eeos.elf eeos.dis .tmp.dtb.o

distclean: clean
	$(Q) echo "  CLEAN   .config include/config"
	$(Q) rm -rf include/config .config .config.old
	$(Q) echo "  CLEAN   tags cscope.in.out cscope.out cscope.po.out"
	$(Q) rm -f tags cscope.in.out cscope.out cscope.po.out

scriptconfig:
	$(Q)$(kpython) $(SCRIPT) $(Kconfig) $(if $(SCRIPT_ARG),"$(SCRIPT_ARG)")

iscriptconfig:
	$(Q)$(kpython) -i -c \
	  "import kconfiglib; \
	   kconf = kconfiglib.Kconfig('$(Kconfig)'); \
	   print('A Kconfig instance \'kconf\' for the architecture $(ARCH) has been created.')"

menuconfig:
	$(Q)$(kpython) $(srctree)/scripts/Kconfiglib/menuconfig.py $(Kconfig)

guiconfig:
	$(Q)$(kpython) $(srctree)/scripts/Kconfiglib/guiconfig.py $(Kconfig)

dumpvarsconfig:
	$(Q)$(kpython) $(srctree)/scripts/Kconfiglib/examples/dumpvars.py $(Kconfig)

genconfig:
	$(Q)$(kpython) $(srctree)/scripts/Kconfiglib/genconfig.py $(Kconfig)

%defconfig:
	$(Q)test -e configs/$@ || (		\
	echo >&2;			\
	echo >&2 "  ERROR: $@ doest exist.";		\
	echo >&2 ;							\
	/bin/false)
	$(Q) echo "  GEN      .config From configs/$@"
	$(Q) mkdir -p include/config
	$(Q) python $(srctree)/scripts/Kconfiglib/defconfig.py $(Kconfig) configs/$@

PHONY += qemu

ifeq ("$(DEBUG_USE_GDB)", "1")
	QEMU_FLAG = -S -s
else
	QEMU_FLAG =
endif

qemu: eeos
ifeq ("$(SRCARCH)", "aarch64")
	qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 2048 -nographic \
		-serial mon:stdio -kernel $(MBUILD_IMAGE) $(QEMU_FLAG)
endif
ifeq ("$(SRCARCH)", "riscv64")
	qemu-system-riscv64 -machine virt -smp 1 -m 1024 -nographic -serial mon:stdio -bios $(MBUILD_IMAGE) $(QEMU_FLAG)
endif
ifeq ("$(SRCARCH)", "riscv32")
	qemu-system-riscv32 -machine virt -smp 1 -m 1024 -nographic -serial mon:stdio -bios $(MBUILD_IMAGE_ELF) $(QEMU_FLAG)
endif

PHONY += FORCE
FORCE:

# Declare the contents of the PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friend)
.PHONY: $(PHONY)
