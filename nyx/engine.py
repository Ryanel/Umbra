from posixpath import curdir, split
from graphlib import TopologicalSorter
import sys
import json
from nyx.package import *

class BuildEngine:
    """Manages the build process for the repository"""

    def __init__(self, json, config: dict, args: dict):
        self.json = json
        self.config = config
        self.packages = dict()
        self.enviroment = dict()
        self.args = args

    def load_packages(self):
        for x in self.json['packages']:
            pkg_json = self.json['packages'][x]
            pkg = NyxPackage(x)
            pkg.loadFromJson(pkg_json, self.config)
            self.packages[x] = pkg
        for y in self.json['includes']:
            with open(y) as repo_file:
                dat = json.load(repo_file)
                for x in dat['packages']:
                    pkg_json = dat['packages'][x]
                    pkg = NyxPackage(x)
                    pkg.loadFromJson(pkg_json, self.config)
                    self.packages[x] = pkg

    def uncache(self, pkg_name):
        for pkg in self.packages:
            if pkg == pkg_name:
                self.packages[pkg].cached = False
                # Now, search for dependancies
                for dpkg in self.packages:
                    for dpkg_req in self.packages[dpkg].requirements:
                        if dpkg_req == pkg_name:
                            self.packages[dpkg].cached = False
                            self.uncache(dpkg)

    def clean(self):
        self.clean_build_root()
        shutil.rmtree(os.path.abspath(self.config["sysroot"]), ignore_errors=False)
        shutil.rmtree(os.path.abspath(self.config["package_root"]), ignore_errors=False)

    def clean_build_root(self):
        shutil.rmtree(os.path.abspath(self.config["build_root"]), ignore_errors=False)

    def run(self):
        subprocess.run(['./nyx/scripts/x86_64-create-iso.sh'], shell=True, stdout=sys.stdout)
        subprocess.run(self.json['general']['run_command'], shell=True, stdout=sys.stdout)

    def debug(self):
        subprocess.run(self.json['general']['debug_command'], shell=True, stdout=sys.stdout)

    def build(self, step = ''):
        dep_graph = TopologicalSorter()
        for x, y in self.packages.items():
            dep_graph.add(x, *y.requirements)

        compile_order = [*dep_graph.static_order()]

        packages_to_compile_in_order = []
        for x in compile_order:
            packages_to_compile_in_order.append(self.packages[x])

        if step == 'reinstall':
            for pkg in packages_to_compile_in_order:
                if (not pkg.install(self.config)):
                    print(f"Failed to install {pkg.name}!")
                    return 1
                print(f"[nyx]: Installing {pkg.name}-{pkg.version}")
            return 0

        for pkg in packages_to_compile_in_order:
            if (pkg.cached):
                print(f"[nyx]: Using cached version of package {pkg.name}-{pkg.version}")
                continue
            print(f"[nyx]: Compiling {pkg.name}-{pkg.version}")
            # Fetch
            if (not pkg.fetch(self.config)):
                print(f"Failed to fetch {pkg.name}!")
                return 1
            # Patch
            if (not pkg.patch(self.config)):
                print(f"Failed to patch {pkg.name}!")
                return 1
            # Configure
            if (not pkg.configure(self.config, self.enviroment)):
                print(f"Failed to configure {pkg.name}!")
                return 1
            # Build
            if (not pkg.build(self.config, self.enviroment)):
                print(f"Failed to build {pkg.name}!")
                return 1
            # Package
            if (not pkg.package(self.config, self.enviroment)):
                print(f"Failed to package {pkg.name}!")
                return 1
            if (not pkg.install(self.config)):
                print(f"Failed to install {pkg.name}!")
                return 1
        return 0