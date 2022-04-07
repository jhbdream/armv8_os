CPPFLAGS += 
CFLAGS += -march=rv64im -mabi=lp64 -nostdinc -fno-stack-protector 
ASFLAGS += -march=rv64im -mabi=lp64 -nostdinc -fno-stack-protector 
LDFLAGS += -Wl,--build-id=none -march=rv64im -mabi=lp64 -fno-stack-protector -static -nostartfiles -nostdlib -ffreestanding -fno-common
