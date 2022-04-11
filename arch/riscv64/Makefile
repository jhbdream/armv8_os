ARCH_CFLAGS += -march=rv64im -mabi=lp64 -nostdinc -fno-stack-protector
ARCH_ASFLAGS += -march=rv64im -mabi=lp64 -nostdinc -fno-stack-protector
ARCH_LDFLAGS += -Wl,--build-id=none -march=rv64im -mabi=lp64 -fno-stack-protector -static -nostartfiles -nostdlib -ffreestanding -fno-common

head-y += arch/riscv64/kernel/startup.o
core-y += arch/riscv64/kernel/
