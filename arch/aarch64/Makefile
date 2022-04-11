ARCH_CFLAGS += -fno-stack-protector -nostdinc
ARCH_ASFLAGS += -fno-stack-protector -nostdinc
ARCH_LDFLAGS += -Wl,--build-id=none -fno-stack-protector -static -nostartfiles -nostdlib -ffreestanding -fno-common

head-y += arch/aarch64/boot/startup.o
core-y += arch/aarch64/boot/
