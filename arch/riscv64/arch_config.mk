CPPFLAGS += 
CFLAGS += -march=rv64g -nostdinc -fno-stack-protector 
ASFLAGS += 
LDFLAGS += -Wl,--build-id=none -fno-stack-protector -static -nostartfiles -nostdlib -ffreestanding -fno-common
