echo "iso: Generating LiveCD ISO..."
cp -r ./media/x86/x86_64/* /opt/umbra-buildenv/artifacts/sysroot/

mkdir /opt/umbra-buildenv/artifacts/initrd/ -p
cp -r ./media/initrd/* /opt/umbra-buildenv/artifacts/initrd/
find /opt/umbra-buildenv/artifacts/initrd -printf "%P\n" -type f -o -type l -o -type d | tar -cf /opt/umbra-buildenv/artifacts/sysroot/boot/initrd.tar --format=ustar --no-recursion -C /opt/umbra-buildenv/artifacts/initrd/ -T -

cd /opt/umbra-buildenv/build/tools/limine/
cp limine.sys limine-cd.bin limine-eltorito-efi.bin /opt/umbra-buildenv/artifacts/sysroot/
cd /opt/umbra-buildenv/artifacts/

xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-eltorito-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        sysroot -o livecd.iso 2> /dev/null

./boot/limine/limine-install livecd.iso 2> /dev/null

exit 0