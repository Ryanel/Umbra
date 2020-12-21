# Umbra
Umbra is a operating system for x86 and ARMv8 (64-bit). It is under active development.
It is a full operating system, consisting of a custom C library, custom kernel, and custom userspace.
It takes cues from UNIX systems but is not UNIX compliant.

The build system is CMake with helper scripts. It uses (by default) Ninja for compilation. Documentation can be generated with doxygen in the main root directory.

## Major Components

* Kernel: /sys/kernel ; The heart of the operating system, the interface to the hardware. This is a hybrid kernel.
* Libc : /lib/c/ ; The C Library. Umbra uses it's own C library.
* Loader: /boot/loader ; Loads the operating system on systems which cannot directly boot the Umbra kernel.

## Building

Requirements:
* A cross compiler for the platform of choice, including binutils.
    * i686-elf-gcc or compatible for x86
    * aarch64-elf or compatibile for ARMv8 based platforms
    * Clang, though this is not well tested.
* CMake 
* (optional) Doxygen for documentation.

To build the operating system:

1. Create the ./build directory with `mkdir -p build`
2. Invoke ../configure [name of configuration file], with the configuration file being a CMake toolchain file inside of the ./config folder, without the .cmake. This selects both the architecture and platform.
3. Invoke `ninja` to build the operating system
4. Invoke `ninja install` to populate the system root.
5. To build a certain type of media, invoke the script in scripts. For x86, invoke x86-create-iso to make an iso. This also builds an up to date image beforehand.