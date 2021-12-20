#!/usr/bin/env python

# Nyx build coordinator
# By Ryanel

import os
import sys
import subprocess
import argparse
import json
import shutil
import json

from nyx.package import *

# What Nyx will do:
# 1. Build a graph of all .nyx files in the src directory
# 2. Fetch source files (if needed)
# 3. Patch source files (if requested)
# 4. Configure
# 5. Build to a binary package
# 6. Install into system root.

# Downloaded files will be in cache
# Final sysroot is in sysroot
# Temp is where build files go. It follows the format temp/{dir}/{package-name}-{package-version}/
# Packages is where installed packages go.

config = dict()

def main() -> int:

    limine = NyxPackage("limine")
    limine.isTool = True
    limine.architecture = 'x86_64'
    limine.src_path = ""
    limine.acquisition = 'git'
    limine.git_repository = 'https://github.com/limine-bootloader/limine.git'
    limine.git_branch = 'v2.0-branch-binary'
    limine.src_path = "tools/limine"
    limine.build_steps = [
        'make -C limine'
    ]
    limine.package_steps = []

    libc = NyxPackage("libc")
    libc.src_path = "lib/c"
    libc.installroot = "/"

    kernel = NyxPackage("kernel")
    kernel.src_path = "sys/kernel"
    kernel.installroot = "/sys/"

    packages_to_compile_in_order = [limine, libc, kernel]

    config["source_root"] = "src/"
    config["sysroot"] = "build/sysroot/"
    config["build_root"] = "build/temp/"
    config["package_root"] = "build/packages/"

    common_enviroment = dict()
    common_enviroment['PATH'] = os.environ.get("PATH")
    common_enviroment['CC'] = 'x86_64-elf-gcc'
    common_enviroment['CXX'] = 'x86_64-elf-g++'
    common_enviroment['AR'] = 'x86_64-elf-ar'
    common_enviroment['NYX_ARCH'] = 'x86'
    common_enviroment['NYX_TARGET'] = 'x86_64'
    common_enviroment['NYX_TARGET_TRIPLE'] = 'x86_64-elf-umbra'
    common_enviroment['HOST_CC'] = str(os.environ.get("CC"))
    common_enviroment['HOST_CPP'] = str(os.environ.get("CPP"))

    for pkg in packages_to_compile_in_order:
        print(f"Compiling {pkg.name}-{pkg.version}")
        # Fetch
        if (not pkg.fetch(config)):
            print(f"Failed to fetch {pkg.name}!")
            return 1
        # Patch
        if (not pkg.patch(config)):
            print(f"Failed to patch {pkg.name}!")
            return 1
        # Configure
        if (not pkg.configure(config, common_enviroment)):
            print(f"Failed to configure {pkg.name}!")
            return 1
        # Build
        if (not pkg.build(config, common_enviroment)):
            print(f"Failed to build {pkg.name}!")
            return 1
        # Package
        if (not pkg.package(config, common_enviroment)):
            print(f"Failed to package {pkg.name}!")
            return 1
        if (not pkg.install(config)):
            print(f"Failed to install {pkg.name}!")
            return 1

    subprocess.run(['./nyx/scripts/x86_64-create-iso.sh'], shell=True, stdout=sys.stdout)
    subprocess.run(['./nyx/scripts/x86-run.sh'], shell=True, stdout=sys.stdout)
    return 0

# Make this file executable...
if __name__ == '__main__':
    sys.exit(main())