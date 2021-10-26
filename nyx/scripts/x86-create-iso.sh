echo "Build: Adding LiveCD files to system root..."
cp -r ./media/x86/i686/* build/sysroot/

echo "Build: Generating LiveCD ISO..."
grub-mkrescue -o build/livecd.iso build/sysroot --quiet >> /dev/null
