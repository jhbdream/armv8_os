CPPFLAGS += 
CFLAGS += -fno-stack-protector -nostdinc
ASFLAGS += 
LDFLAGS += -Wl,--build-id=none -fno-stack-protector -static -nostartfiles -nostdlib -ffreestanding -fno-common 
