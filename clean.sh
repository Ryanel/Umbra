#!/bin/bash

cd build
cd boot
ninja clean
cd ..

cd src
ninja clean
cd ..

cd sysroot
rm -r *
cd ..
cd ..