echo "Build: Adding LiveCD files to system root..."
cp -r ./media/x86/x86_64/* build/sysroot/

echo "Build: Creating initial ramdisk..."
mkdir build/initrd/ -p
cp -r ./media/initrd/* build/initrd/
find build/initrd/ -printf "%P\n" -type f -o -type l -o -type d | tar -cf build/sysroot/boot/initrd.tar --format=ustar --no-recursion -C build/initrd/ -T -

echo "Build: Generating LiveCD ISO..."

cd build/limine/
cp limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin ../sysroot/
cd ../

xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-eltorito-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        sysroot -o livecd.iso 2> /dev/null

./limine/limine/limine-install livecd.iso 2> /dev/null