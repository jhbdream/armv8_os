qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 2048 -nographic -serial mon:stdio -kernel build/app.bin
