#!/bin/bash

# There's multiple source trees to build for a Umbra OS install:
# /boot
# /sys
# /lib
# /bin

BUILD_CMD="ninja"
BUILD_DIR="build"
cd $BUILD_DIR

# /boot
echo "Build: /boot"
cd boot
$BUILD_CMD
$BUILD_CMD install
cd ..

# /src
echo "Build: /*"
cd src
$BUILD_CMD
$BUILD_CMD install
cd ..

cd ..

# Step 3: Generate Media
echo "Build: Generate media"
if [ -f "$BUILD_DIR/config/x86_64" ]; then
    ./scripts/x86-create-iso.sh
fi

if [ -f "$BUILD_DIR/config/i686" ]; then
    ./scripts/x86-create-iso.sh
fi
