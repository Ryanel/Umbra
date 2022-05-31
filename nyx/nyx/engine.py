from concurrent.futures import process
import functools
from nyx.package import *
from nyx.globals import *
from .json import nyx_write_json
from .json import nyx_read_json

from graphlib import TopologicalSorter
import docker
from rich.panel import Panel
from rich.live import Live
from rich.pretty import Pretty  
from rich import print

class Engine:
    """Contains the world, the collection of all packages"""
    def __init__(self, build_env):
        self.packages = dict()
        self.environment = dict()
        self.repo_json = {}
        self.config = dict()
        self.saved_state = dict()
        self.saved_state["packages"] = dict()
        self.load_environment()
        self.build_env = build_env

    def load_packages(self, repo_path: str):
        """Loads data from the repository"""
        nyx_log.debug("Loading data from the repository...");

        self.repo_path = repo_path
        self.repo_json = nyx_read_json(repo_path + "meta.json");

        for category in os.listdir(self.repo_path):
            category_dir = os.path.join(self.repo_path, category)
            if not os.path.isdir(category_dir):
                continue
            self.load_package_dir(category, category_dir)
            

    def load_package_dir(self, cat: str, pkg_dir: str):
        for cat_pkgs in os.listdir(pkg_dir):
            full_path = os.path.join(pkg_dir, cat_pkgs)
            if not os.path.isdir(full_path):
                split = os.path.splitext(cat_pkgs)
                full_name = split[0]

                if (split[1] == ".json"):
                    # Actually add the package
                    pkg_json = nyx_read_json(full_path)
                    pkg = NyxPackage(full_name, pkg_dir)
                    pkg.loadJson(pkg_json)
                    self.packages[full_name] = pkg
            else:
                self.load_package_dir(cat, full_path)


    def load_state(self, state_path: str):
        self.saved_state = nyx_read_json(state_path, self.saved_state);
        for x in self.saved_state["packages"]:
            self.packages[x].state = self.saved_state['packages'][x]


    def save_state(self, state_path: str):
        for x in self.packages:
            self.saved_state["packages"][x] = self.packages[x].state
        
        nyx_write_json(state_path, self.saved_state)


    def set_config(self, config: dict) -> None:
        self.config = config


    def load_environment(self):
        triple="x86_64-umbra"
        self.environment = dict()
        self.environment['PATH'] = os.environ.get("PATH")
        self.environment |= {
            "CC": f"{triple}-gcc",
            "CC": f"{triple}-gcc",
            "CXX": f"{triple}-g++",
            "AR": f"{triple}-ar",
            "ASM": "nasm",
            "ASMFLAGS": "-f elf64 -F dwarf -g",
            "NYX_ARCH": "x86",
            "NYX_TARGET": "x86_64",
            "NYX_TARGET_TRIPLE": f"{triple}"
        }


    def clean(self):
        """Cleans the build directory."""
        pass


    def prerequesites(self):
        """Ensures prerequesites are setup correctly"""
        pass


    def run(self, config:dict):
        self.coordinator_run_command(config, "bash -c 'cd /opt/umbra-buildenv/src/ && ./nyx/files/scripts/x86_64-create-iso.sh'")
        try:
            subprocess.run(['qemu-system-x86_64' ,'-cdrom', 'artifacts/livecd.iso', '-serial', 'stdio'])
        except KeyboardInterrupt:
            nyx_log.info("QEMU closed")
        

    def get_dependencies(self, pkg: NyxPackage, shallow=False) -> set():
        # Recursively get the dependencies...
        dependencies = set()
        for x in pkg.dependencies:
            req_pkg = self.query_package(x, latest_if_no_version=True)

            if len(req_pkg) == 1:
                dependencies.add(req_pkg[0])
                if not shallow:
                    dependencies.update(self.get_dependencies(req_pkg[0]))

            elif len(req_pkg) == 0:
                nyx_log.error(f"{pkg.name}'s dependency {x} matches no packages")
            else:
                nyx_log.error(f"{pkg.name}'s dependency {x} matches multiple packages")

        return dependencies


    def sort_install_order(self, packages: set) -> list:
        dep_graph = TopologicalSorter()
        for x in packages:
            dep_graph.add(x, *self.get_dependencies(x, shallow=True)) # Does not handle versions...

        compile_order = [*dep_graph.static_order()]
        return compile_order


    def query_package(self, pkg_str: str, latest_if_no_version:bool = True) -> list:
        """Searches the world for packages matching pkg_str."""
        found_pkgs = list()

        search_name = NyxPackage.pkgstr_name(pkg_str)
        search_ver  = NyxPackage.pkgstr_version(pkg_str)

        for candidate in self.packages:
            add_this_package = False
            candidate_name = NyxPackage.pkgstr_name(candidate)
            candidate_ver = NyxPackage.pkgstr_version(candidate)

            # print(f"pstr {pkg_str} ({search_name}-{search_ver}) candidate: ({candidate_name}-{candidate_ver})")

            if candidate_name != search_name:
                if candidate_name != (f"{search_name}-{search_ver}"):
                    continue
                else:
                    # Handle the case where there's multiple - in a name...
                    search_ver = None

            # Is there an exact version?
            if search_ver != None:
                # No, compare versions
                if candidate_ver == search_ver:
                    add_this_package = True
            elif latest_if_no_version:
                add_this_package = True

            if add_this_package:
                found_pkgs.append(self.packages.get(candidate))

        if latest_if_no_version and search_ver == None:
            if len(found_pkgs) <= 1:
                return found_pkgs
            sorted_version = sorted(found_pkgs, key=functools.cmp_to_key(NyxPackage.compareVersions))
            return [sorted_version[-1]]

        return found_pkgs


    def coordinator_run_command(self, config:dict, command:str):
        if (self.build_env == "docker"):
            client = docker.from_env()
            real_src_path = os.path.abspath(config["host_env"]["source_path"])
            real_artifact_path = os.path.abspath(config["host_env"]["artifact_path"])
            container = client.containers.run('umbra-buildenv', 
                command, 
                detach = True,
                stderr = True,
                stdout = True,
                remove = True,
                volumes={
                    real_src_path: {'bind': '/opt/umbra-buildenv/src', 'mode': 'rw'},
                    'umbra_build': {'bind': '/opt/umbra-buildenv/build', 'mode': 'rw'},
                    real_artifact_path: {'bind': '/opt/umbra-buildenv/artifacts', 'mode': 'rw'}
                }
            )

            log_output = []
            list_size = os.get_terminal_size().lines - 10

            if list_size < 5:
                list_size = 5

            if list_size > 15:
                list_size = 15

            # log_panel = Panel("", title="Log")
            # with Live(log_panel, refresh_per_second=10, console=nyx_log.console) as live:
            #     for line in container.logs(stream=True):
            #         log_output.append(line)

            #         toDisplay = log_output[-(list_size - 2):]
            #         text = ""

            #         for x in toDisplay:
            #             text = text + x.decode("utf-8")

            #         live.update(Panel(text, title="Log"))


            for line in container.logs(stream=True):
                print(line.decode("utf-8").strip())

            container.wait()


    def coordinator_build_package(self, config:dict, pkg: NyxPackage, rebuild:bool ):
        nyx_log.info (f"Compiling {pkg.name}")
        if (self.build_env == "docker"):
            command = './nyx/nbuild.py --no-color '
            if rebuild: 
                command = command + '--clean ' 
            command = command + f'install {pkg.name}-{pkg.version}'
            nyx_log.info (f"Compiling {command}")
            self.coordinator_run_command(config, command)
            