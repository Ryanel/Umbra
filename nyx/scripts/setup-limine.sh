mkdir -p  build/limine/

cd build/limine
git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1

make -C limine