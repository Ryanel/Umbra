ninja install

echo "Adding LiveCD files to system root..."
cp -r ../media/x86/i686/* sysroot/

echo "Generating LiveCD ISO..."
grub-mkrescue -o livecd.iso sysroot >> /dev/null
