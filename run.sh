BUILD_DIR="build"

./build.sh

if [ -f "$BUILD_DIR/config/x86_64" ]; then
    ./scripts/x86-run.sh
fi
