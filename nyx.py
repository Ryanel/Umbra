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
from graphlib import TopologicalSorter

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
repo_json = None
def read_repo():
    repo_file = open('repo.json')
    dat = json.load(repo_file)
    repo_file.close()
    return dat

def read_packages(repo_json):
    all_packages = dict()
    for x in repo_json['packages']:
        pkg_json = repo_json['packages'][x]
        pkg = NyxPackage(x)
        pkg.nice_name       = pkg_json["name"] or x
        pkg.architecture    = pkg_json["architecture"] or "all"
        pkg.version         = pkg_json.get("version", "dev")
        pkg.acquisition     = pkg_json.get("acquisition", "local")
        pkg.src_path        = pkg_json.get("src_path", "")
        pkg.tools           = pkg_json.get("tools", [])
        pkg.requirements    = pkg_json.get("requirements", [])
        pkg.patches         = pkg_json.get("patches", [])
        pkg.configure_steps = pkg_json.get("configure_steps", [])
        pkg.build_steps     = pkg_json.get("build_steps", [])
        pkg.package_steps   = pkg_json.get("package_steps", [])
        pkg.installroot     = pkg_json.get("install_root", "/")
        pkg.enviroment      = pkg_json.get("enviroment", dict())
        pkg.isTool          = bool(pkg_json.get("is_tool", False))
        pkg.git_branch      = pkg_json.get("git_branch", "master")
        all_packages[x] = pkg
    return all_packages


def nyx_build(packages, common_environment) -> int:
    dep_graph = TopologicalSorter()
    for x, y in packages.items():
        dep_graph.add(x, *y.requirements)

    compile_order = [*dep_graph.static_order()]


    packages_to_compile_in_order = []
    for x in compile_order:
        packages_to_compile_in_order.append(packages[x])

    for pkg in packages_to_compile_in_order:
        print(f"[nyx]: Compiling {pkg.name}-{pkg.version}")
        # Fetch
        if (not pkg.fetch(config)):
            print(f"Failed to fetch {pkg.name}!")
            return 1
        # Patch
        if (not pkg.patch(config)):
            print(f"Failed to patch {pkg.name}!")
            return 1
        # Configure
        if (not pkg.configure(config, common_environment)):
            print(f"Failed to configure {pkg.name}!")
            return 1
        # Build
        if (not pkg.build(config, common_environment)):
            print(f"Failed to build {pkg.name}!")
            return 1
        # Package
        if (not pkg.package(config, common_environment)):
            print(f"Failed to package {pkg.name}!")
            return 1
        if (not pkg.install(config)):
            print(f"Failed to install {pkg.name}!")
            return 1
    return 0

def main() -> int:
    global config
    global repo_json
    # Arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("module", help="The module to execute. Can be clean, build, run")
    parser.add_argument("-r","--run", help="Run an image, if possible", action="store_true")

    args = parser.parse_args()
    config['run_on_completion'] = args.run or False

    repo_json = read_repo()

    # Read configuration from the repo.json file
    common_environment = dict()
    common_environment['PATH'] = os.environ.get("PATH")
    common_environment |= repo_json['general']['env']
    config["source_root"] = repo_json['general']['source_root']
    config["sysroot"] = repo_json['general']['sysroot']
    config["build_root"] = repo_json['general']['build_root']
    config["package_root"] = repo_json['general']['package_root']

    # Read in a list of packages...
    all_packages = read_packages(repo_json)

    if (args.module == 'build'):
        if (nyx_build(all_packages, common_environment) == 0) and config['run_on_completion']:
            subprocess.run(['./nyx/scripts/x86_64-create-iso.sh'], shell=True, stdout=sys.stdout)
            subprocess.run(['./nyx/scripts/x86-run.sh'], shell=True, stdout=sys.stdout)
    elif (args.module == 'clean'):
        # Just delete the build directory
        shutil.rmtree(os.path.abspath(config["build_root"]), ignore_errors=False)
    return 0

# Make this file executable...
if __name__ == '__main__':
    sys.exit(main())