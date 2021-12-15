echo "Build: Adding LiveCD files to system root..."
cp -r ./media/x86/i686/* build/sysroot/

echo "Build: Creating initial ramdisk..."
mkdir build/initrd/ -p
cp -r ./media/initrd/* build/initrd/
find build/initrd/ -printf "%P\n" -type f -o -type l -o -type d | tar -cf build/sysroot/boot/initrd.tar --format=ustar --no-recursion -C build/initrd/ -T -

echo "Build: Generating LiveCD ISO..."
grub-mkrescue -o build/livecd.iso build/sysroot --quiet >> /dev/null 2> /dev/null
