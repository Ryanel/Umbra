from nyx.package import *
from nyx.globals import *
import nyx.json;
from graphlib import TopologicalSorter
import docker
from rich.panel import Panel
from rich.live import Live
from rich.pretty import Pretty
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
        self.repo_json = nyx.json.read_json(repo_path + "repo.json");

        for x in self.repo_json['packages']:
            pkg_json = self.repo_json['packages'][x]
            pkg = NyxPackage(x)
            pkg.loadJson(pkg_json)
            self.packages[x] = pkg
        for y in self.repo_json['includes']:
            add_path = self.repo_path + y
            dat = nyx.json.read_json(add_path)
            for x in dat['packages']:
                pkg_json = dat['packages'][x]
                pkg = NyxPackage(x)
                pkg.loadJson(pkg_json)
                self.packages[x] = pkg
        pass

    def load_state(self, state_path: str):
        self.saved_state = nyx.json.read_json(state_path, self.saved_state);
        for x in self.saved_state["packages"]:
            self.packages[x].state = self.saved_state['packages'][x]

    def save_state(self, state_path: str):
        for x in self.packages:
            self.saved_state["packages"][x] = self.packages[x].state
        
        nyx.json.write_json(state_path, self.saved_state)

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

        # For now, do not care about versions, just look for pkg_str in packages
        pkg = self.packages.get(pkg_str)
        if (pkg != None):
            found_pkgs.append(pkg)

        return found_pkgs


    def coordinator_build_package(self, config:dict, pkg: NyxPackage, rebuild:bool ):
        if (self.build_env == "docker"):
            client = docker.from_env()

            real_src_path = os.path.abspath(config["host_env"]["source_path"])
            real_artifact_path = os.path.abspath(config["host_env"]["artifact_path"])

            nyx_log.info (f"Compiling {pkg.name}")

            command = './nbuild.py --no-color '

            if rebuild:
                command = command + '--clean ' 
            command = command + f'install {pkg.name}'
            container = client.containers.run('umbra-buildenv', 
                command, 
                #'ls -la  /opt/umbra-buildenv/build/',
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

            log_panel = Panel("", title="Log")
            with Live(log_panel, refresh_per_second=10, console=nyx_log.console) as live:
                for line in container.logs(stream=True):
                    log_output.append(line)

                    toDisplay = log_output[-(list_size - 2):]
                    text = ""

                    for x in toDisplay:
                        text = text + x.decode("utf-8")

                    live.update(Panel(text, title="Log"))
            container.wait()
