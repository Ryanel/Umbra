qemu-system-x86_64 -cdrom build/livecd.iso -serial stdio -m 128 -cpu max -machine type=q35 -vga cirrus -smp 1 -no-reboot -no-shutdown -s -S