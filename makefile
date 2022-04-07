ARCH =aarch64
CROSS_COMPILE =aarch64-linux-gnu-

# Other switches the user should not normally need to change:
OUTPUT_IMAGE = os
DEBUG_FLAGS = -g
QUIET ?=@
OPT_LEVEL ?= 0
CPPFLAGS_EXTRA :=

ifeq ($(QUIET),@)
PROGRESS = @echo Compiling $<...
endif

CC = $(CROSS_COMPILE)gcc
OC = $(CROSS_COMPILE)objcopy
OD = $(CROSS_COMPILE)objdump

LD_FILE = arch/$(ARCH)/ld_script/kernel.ld

# Commit hash from git
COMMIT=$(shell git rev-parse --short HEAD)

export SRC_DIR
export INCLUDES

OBJ_DIR := obj

SRC_DIR := arch/$(ARCH)
INCLUDES := -Iarch/$(ARCH)/include

INCLUDES += -Iinclude 	\
			-Iinclude/kernel	\
			-Iinclude/libc \
			-Iinclude/ee

SRC_DIR += 	user	\
			common/libc	\
			common/time	\
			common/interrupt	\
			driver/uart	\
			driver/irq	\
			driver/irq/gicv3	\
			kernel			\
			kernel/mm \
			lib	\
			kernel/printk

include common/letter-shell/liblettelshell.mk
include common/printf_format/printf_format.mk

define EOL =

endef

RM_FILES = $(foreach file,$(1),rm -f $(file)$(EOL))
RM_DIRS = $(foreach dir,$(1),rm -rf $(dir)$(EOL))

DEPEND_FLAGS = -MD -MF $@.d

CPPFLAGS := $(DEFINES) $(INCLUDES) $(DEPEND_FLAGS)
CFLAGS := $(DEBUG_FLAGS) -O$(OPT_LEVEL)
ASFLAGS := $(DEBUG_FLAGS)
LDFLAGS := -T$(LD_FILE)
TARGET_ARCH :=

export CFLAGS ASFLAGS LDFLAGS DEFINES
include arch/$(ARCH)/arch_config.mk

#mkdir 创建输出文件目录
X_OUTPUT_DIRS := $(patsubst %, $(OBJ_DIR)/%, $(SRC_DIR))

APP_C_SRC	:=	$(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c))
APP_S_SRC	:=	$(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.S))
OBJ_FILES	:=	$(patsubst %, $(OBJ_DIR)/%, $(APP_C_SRC:.c=.o)) \
				$(patsubst %, $(OBJ_DIR)/%, $(APP_S_SRC:.S=.o))

DEP_FILES := $(OBJ_FILES:%=%.d)

.phony: all clean pack

all: $(OUTPUT_IMAGE)

$(OUTPUT_IMAGE): $(OBJ_FILES) $(LD_FILE)
	@echo Linking $@
	$(QUIET) $(CC) $(TARGET_ARCH) $(LDFLAGS) --output $@ $(OBJ_FILES) -lm -Wl,-Map,$@.map
	@echo Objcopying $@.bin
	$(QUIET) $(OC) -v -O binary $@ $@.bin
	$(QUIET) $(OD) -D $@ > $@.dis
	@echo Done.

clean:
	$(call RM_DIRS,$(OBJ_DIR))
	$(call RM_FILES,$(OUTPUT_IMAGE))
	$(call RM_FILES,$(OUTPUT_IMAGE).bin)
	$(call RM_FILES,$(OUTPUT_IMAGE).dis)

qemu: $(OUTPUT_IMAGE)
	qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 1024 -nographic -serial mon:stdio -kernel $(OUTPUT_IMAGE)

qemu_gdb: $(OUTPUT_IMAGE)
	qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 1024 -nographic -serial mon:stdio -kernel $(OUTPUT_IMAGE) -S -gdb tcp::1234

$(X_OUTPUT_DIRS):
	@mkdir -p $@

$(OBJ_DIR)/%.o : %.c | $(X_OUTPUT_DIRS)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o : %.S | $(X_OUTPUT_DIRS)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(ASFLAGS) -o $@ $<

# Make sure everything is rebuilt if this makefile is changed
$(OBJ_FILES) $(OUTPUT_IMAGE): makefile

-include $(DEP_FILES)
