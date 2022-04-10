# SPDX-License-Identifier: GPL-2.0

src := $(obj)

obj-y :=
subdir-y :=
lib-y :=
targets :=

EXTRA_AFLAGS   :=
EXTRA_CFLAGS   :=
EXTRA_CPPFLAGS :=
EXTRA_LDFLAGS  :=

include scripts/config.mk

# Read auto.conf if it exists, otherwise ignore
-include .config

# The filename Kbuild has precedence over Makefile add Makefile
kbuild-dir := $(if $(filter /%,$(src)),$(src),$(srctree)/$(src))
kbuild-file := $(if $(wildcard $(kbuild-dir)/Kbuild),$(kbuild-dir)/Kbuild,$(kbuild-dir)/Makefile)
include $(kbuild-file)

ifndef obj
$(warning kbuild: Minos.mk is included improperly)
endif

# obj-y 变量中 如果结尾带了 / 表示是目录
__subdir-y	:= $(patsubst %/,%,$(filter %/, $(obj-y)))
subdir-y	+= $(__subdir-y)
obj-y		:= $(patsubst %/, %/built-in.o, $(obj-y))

# $(subdir-obj-y) is the list of objects in $(obj-y) which uses dir/ to
# tell kbuild to descend
subdir-obj-y := $(filter %/built-in.o, $(obj-y))

# Replace multi-part objects by their individual parts,
# including built-in.o from subdirectories
real-obj-y := $(foreach m, $(obj-y), $(if $(strip $($(m:.o=-objs)) $($(m:.o=-y))),$($(m:.o=-objs)) $($(m:.o=-y)),$(m)))

# generate the dependece file
real-dep-y := $(addsuffix .d, $(real-obj-y))
real-dep-y := $(addprefix $(obj)/., $(real-dep-y))

# Add subdir path
subdir-obj-y	:= $(addprefix $(obj)/,$(subdir-obj-y))
real-obj-y	:= $(addprefix $(obj)/,$(real-obj-y))

# remove the .lds file which is not link to built-in.o
lds-y		:= $(filter %.lds, $(real-obj-y))
real-obj-y	:= $(filter-out %.lds, $(real-obj-y))

# remove the .dtb file which is not link to built-in.o
dtb-y		:= $(filter %.dtb, $(real-obj-y))
real-obj-y	:= $(filter-out %.dtb, $(real-obj-y))

# remove the head-y which is link as the first.o
real-head-y	:= $(filter $(MBUILD_EEOS_INIT), $(real-obj-y))
real-obj-y	:= $(filter-out $(MBUILD_EEOS_INIT), $(real-obj-y))

OBJ_DEPS	= $(real-dep-y)

CFLAGS		= $(MBUILD_CFLAGS) $(ARCH_CFLAGS) $(EXTRA_CFLAGS)
CFLAGS		+= -MMD -MF $(@D)/.$(@F).d

AFLAGS 		= $(MBUILD_AFLAGS) $(ARCH_AFLAGS) $(EXTRA_AFLAGS)

builtin-target 	:= $(obj)/built-in.o

__build: $(subdir-y) $(builtin-target) $(lds-y) $(real-head-y)
	@:

ifdef builtin-target
$(builtin-target): $(real-obj-y)
ifeq ($(real-obj-y),)
	$(Q) echo "  CC      $@"
	$(Q) $(CC) $(CFLAGS) -c -x c /dev/null -o $@
else
	$(Q) echo "  LD      $@"
	$(Q) $(LD) $(MBUILD_LDFLAGS) -r -o $@ $^
endif

-include $(OBJ_DEPS)

%.o: %.c $(obj)/Makefile $(srctree)/Makefile
	$(Q) echo "  CC      $@"
	$(Q) $(CC) $(CFLAGS) -c $< -o $@

%.o: %.S $(obj)/Makefile $(srctree)/Makefile
	$(Q) echo "  CC      $@"
	$(Q) $(CC) $(AFLAGS) -c $< -o $@

%.lds: %.lds.S $(obj)/Makefile $(srctree)/Makefile
	$(Q) echo "  CC      $@"
	$(Q) $(CC) $(MBUILD_CFLAGS) -E -P $< -o $@

%.dtb: %.dts $(obj)/Makefile $(srctree)/Makefile
	$(Q) echo "  DTC     $@"
	$(Q) $(DTC) -q -I dts -O dtb -o $@ $<

# 处理子目录中的makefile
PHONY += $(subdir-y)
$(subdir-y):
	$(Q) $(MAKE) $(build)=$(obj)/$@

endif

PHONY += FORCE
FORCE:

.PHONY: $(FORCE)
