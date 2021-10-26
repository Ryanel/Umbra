# Umbra

Umbra is a operating system for x86 and ARMv8 (64-bit). It is under active development.
It is a full operating system, consisting of a custom C library, custom kernel, and custom userspace.
It takes cues from UNIX systems but is not UNIX compliant.

The build system is CMake with helper scripts. It uses (by default) Ninja for compilation. Documentation can be generated with doxygen in the main root directory.

## Major Components

- Kernel: /sys/kernel ; The heart of the operating system, the interface to the hardware. This is a hybrid kernel.
- Libc : /lib/c/ ; The C Library. Umbra uses it's own C library.
- Loader: /boot/loader ; Loads the operating system on systems which cannot directly boot the Umbra kernel.

## Building

Requirements:

- A cross compiler for the platform of choice, including binutils.
  - i686-elf-gcc or compatible for x86
  - aarch64-elf or compatibile for ARMv8 based platforms
  - Clang, though this is not well tested.
- CMake
- Python, for the build tool
- (optional) Doxygen for documentation.


To build:

1. Configure the nyx build system with `./nyx.py configure`
    * `--target` sets the target, all available targets are in nyx/targets/. Required to configure.
    * `--build_directory` sets the build directory. Default is build.
    * `--sysroot` sets the system root directory. Default is sysroot.
2. `./nyx.py build` builds the system.
2. `./nyx.py run` runs the system.

Nyx keeps track of all options within it's build directory. Other then target, you can change options by passing them to nyx without a command.