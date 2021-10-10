APP ?= app

QUIET ?=@
OPT_LEVEL ?= 2

# Other switches the user should not normally need to change:
ARCH = armv8-a
DEBUG_FLAGS = -g

CPPFLAGS_EXTRA :=

ifeq ($(QUIET),@)
PROGRESS = @echo Compiling $<...
endif

CROSS_COMPILE := aarch64-none-elf-

CC = $(CROSS_COMPILE)gcc
OC = $(CROSS_COMPILE)objcopy
OD = $(CROSS_COMPILE)objdump

# Commit hash from git
COMMIT=$(shell git rev-parse --short HEAD)
PACK_DATA=$(shell date +'%m%d_%H%M')

export SRC_DIR
export INCLUDES

OBJ_DIR := obj
SRC_DIR := libcpu/aarch64
INCLUDES :=

INCLUDES += -Iinclude 	\
			-Ilibcpu/aarch64/include

SRC_DIR += 	user	\
			common/libc	\
			common/time	\
			common/interrupt	\
			driver/uart	\
			driver/irq	\
			driver/irq/gicv3	\
			kernel

include common/EasyLogger/libEasyLogger.mk

define EOL =

endef

RM_FILES = $(foreach file,$(1),rm -f $(file)$(EOL))
RM_DIRS = $(foreach dir,$(1),rm -rf $(dir)$(EOL))

DEPEND_FLAGS = -MD -MF $@.d

CPPFLAGS = $(DEFINES) $(INCLUDES) $(DEPEND_FLAGS) $(CPPFLAGS_EXTRA)
CFLAGS = $(DEBUG_FLAGS) -O$(OPT_LEVEL)
ASFLAGS = $(DEBUG_FLAGS)
LDFLAGS = -Tgcc.ld -Wl,--build-id=none -nostartfiles -ffreestanding -fno-common $(LDFLAGS_EXTRA)
TARGET_ARCH = -march=$(ARCH)

#mkdir 创建输出文件目录
X_OUTPUT_DIRS :=	$(patsubst %, $(OBJ_DIR)/%, $(SRC_DIR))

APP_C_SRC	:=	$(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c))
APP_S_SRC	:=	$(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.S))

OBJ_FILES	:=	$(patsubst %, $(OBJ_DIR)/%, $(APP_C_SRC:.c=.o)) \
				$(patsubst %, $(OBJ_DIR)/%, $(APP_S_SRC:.S=.o))

DEP_FILES := $(OBJ_FILES:%=%.d)

.phony: all clean pack

all: $(APP)

$(APP): $(OBJ_FILES) gcc.ld
	@echo Linking $@
	$(QUIET) $(CC) $(TARGET_ARCH) $(LDFLAGS) --output $@ $(OBJ_FILES) -lm
	@echo Objcopying $@.bin
	$(QUIET) $(OC) -v -O binary $@ $@.bin
	$(QUIET) $(OD) -D $@ > $@.dis
	@echo Done.

clean:
	$(call RM_DIRS,$(OBJ_DIR))
	$(call RM_FILES,$(APP))
	$(call RM_FILES,$(APP).bin)
	$(call RM_FILES,$(APP).dis)
	$(call RM_FILES,*.tar.gz)

pack:
	@git archive --format tar.gz --output FT2004_Baremetal_$(PACK_DATA)_$(COMMIT).tar.gz master

qemu: $(APP)
	qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 1024 -nographic -serial mon:stdio -kernel $(APP)

qemu_gdb: $(APP)
	qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 1024 -nographic -serial mon:stdio -kernel $(APP) -S -gdb tcp::1234

$(X_OUTPUT_DIRS):
	@mkdir -p $@

$(OBJ_DIR)/%.o : %.c | $(X_OUTPUT_DIRS)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o : %.S | $(X_OUTPUT_DIRS)
	$(PROGRESS)
	$(QUIET) $(CC) -c $(TARGET_ARCH) $(CPPFLAGS) $(ASFLAGS) -o $@ $<

# Make sure everything is rebuilt if this makefile is changed
$(OBJ_FILES) $(APP): makefile

-include $(DEP_FILES)
