#/bin/sh

# Compile OS
ninja install

# Convert kernel.elf to kernel8.img
aarch64-elf-objcopy sysroot/sys/kernel.elf -O binary sysroot/kernel8.img

# Download from offical firmware sources
#mkdir -p cache
#wget -nc https://github.com/raspberrypi/firmware/raw/master/boot/fixup.dat -P cache
#wget -nc https://github.com/raspberrypi/firmware/raw/master/boot/start.elf -P cache

# Copy files to sysroot
#cp cache/fixup.dat sysroot/
#cp cache/start.elf sysroot/

# Run emulator
qemu-system-aarch64 -M raspi3  -kernel sysroot/sys/kernel.elf -serial stdio