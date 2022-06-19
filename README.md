# Umbra

Umbra is a operating system for x86_64 and ARMv8 (64-bit). It is under active development.
It takes cues from UNIX systems but is not UNIX compliant.

This is the monorepo for the Umbra Operating System. 

## Major Components

- Kernel: /sys/kernel ; The heart of the operating system, the interface to the hardware. This is a hybrid kernel.
- Libc : /lib/c/ ; The C Library. Umbra uses it's own C library.
- Loader: /boot/loader ; Loads the operating system on systems which cannot directly boot the Umbra kernel.

## Building

Umbra uses the Nyx build system, which is custom made for Umbra. It is a build coordinator and package manager, rolled into one. 
It is responsible for installing and compiling packages into the system root and creating the operating system image (iso).

It also handles creations of the Toolchain.

### Host requirements
* Docker
* Git
* Python 3.10


### Build Instructions
TODO